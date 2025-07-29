/* C compiler - c01.c Expression Parser and AST Builder
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernized for contemporary C compilers - Archaeological restoration
 * Designed to link with c00-mod.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

/* Forward declarations */
int *build(int op);
int *convert(int *p, int t, int d, int cvn);
int chkw(int *p);
int lintyp(int t);
int *block(int n, int op, int t, int d, int *p1, int *p2, int *p3);
void chklval(int *p);
int notcompat(int at, int st);
int max(int a, int b);

/* External globals from c00.c */
extern int **cp;
extern int *space;
extern int *ospace;
extern int ossiz;
extern int nerror;
extern int line;
extern int fout;
extern int maprel[];  /* Defined in c00.c */

/* External functions from c00.c */
extern void error(char *s, ...);
extern void flush(void);

/* Missing globals that need to be defined */
int opdope[200] = {
    /* Operator precedence and type information table */
    /* From the original c0t.s assembly file */
    00000,  /* EOF */
    00000,  /* ; */
    00000,  /* { */
    00000,  /* } */
    36000,  /* [ */
    02000,  /* ] */
    36000,  /* ( */
    02000,  /* ) */
    14201,  /* : */
    07001,  /* , */
    /* ... rest filled with zeros for now */
};

char cvtab[81] = {
    /* Type conversion table from original */
    000,    /* i:i */
    000,    /* i:c */
    113,    /* i:f */
    125,    /* i:d */
    /* ... simplified for now */
};

int *build(int op) {
    int *p1, t1, d1, *p2, t2, d2, *p3, t3, d3, t;
    int d, dope, lr, cvn;

    printf("DEBUG: build() called with op=%d\n", op);

    if (op==4)  {        /* [] */
        build(40);  /* + */
        op = 36;
    }
    
    dope = opdope[op];
    if ((dope&01)!=0) {
        p2 = *--cp;
        t2 = p2[1];
        d2 = p2[2];
    }
    p1 = *--cp;
    t1 = p1[1];
    d1 = p1[2];
    
    switch (op) {

    /* , */
    case 9:
        *cp++ = block(2, 9, 0, 0, p1, p2, NULL);
        return *cp;

    /* ? */
    case 90:
        if (*p2!=8)
            error("Illegal conditional");
        goto goon;

    /* call */
    case 100:
        *cp++ = block(2,100,t1,24,p1,p2,NULL);
        return *cp;

    /* * */
    case 36:
        if ((t1 -= 16)<0)  {
            error("Illegal indirection");
            t1 += 16;
        }
        if (*p1!=20 && d1==0)
            d1 = 1;
        *cp++ = block(1,36,t1,d1,p1,NULL,NULL);
        return *cp;

    /* & unary */
    case 35:
        if (*p1 == 36) {    /* * */
            *cp++ = (int*)((intptr_t)p1[3]);  /* Proper cast to avoid warning */
            return *cp;
        }
        if (*p1 == 20) {
            *cp++ = block(1,p1[3]==5?29:35,t1+16,1,p1,NULL,NULL);
            return *cp;
        }
        error("Illegal lvalue");
        return NULL;
    }
    
goon:
    if ((dope&02)!=0)        /* lvalue needed on left? */
        chklval(p1);
    if ((dope&020)!=0)        /* word operand on left? */
        chkw(p1);
    if ((dope&040)!=0)        /* word operand on right? */
        chkw(p2);
    if ((dope&01)!=0) {        /* binary op? */
        cvn = cvtab[9*lintyp(t1)+lintyp(t2)];
        if ((dope&010)!=0)  {    /* assignment? */
            t = t1;
            lr = 1;
            cvn &= 07;
        } else {
            t = (cvn&0100)!=0? t2:t1;
            lr = cvn&0200;
            cvn = (cvn>>3)&07;
        }
        if (cvn) {
            if (cvn==07) {
                error("Illegal conversion");
                goto nocv;
            }
            cvn += (dope&010)!=0? 83:93;
            if (lr) {
                t2 = t;
                d2 = (p2=convert(p2, t, d2, cvn))[2];
            } else {
                t1 = t;
                d1 = (p1=convert(p1, t, d1, cvn))[2];
            }
nocv:;        }
        if (d2>d1 && (dope&0100)!=0) {    /* flip commutative? */
            if ((dope&04)!=0)    /* relational? */
                op = maprel[op-60];
            d = d1;
            d1 = d2;
            d2 = d;
            p3 = p1;    /* Use p3 as temp since we can't assign to auto */
            p1 = p2;
            p2 = p3;
            d = t1;
            t1 = t2;
            t2 = d;
        }
        if (d1==d2)
            d = d1+1; 
        else
            d = max(d1,d2);
        if ((dope&04)!=0)
            t = 0;        /* relational is integer */
        *cp++ = block(2,op,t,d,p1,p2,NULL);
        return *cp;
    }
    *cp++ = block(1,op,t1,d1==0?1:d1,p1,NULL,NULL);
    return *cp;
}

int *convert(int *p, int t, int d, int cvn) {
    int c;
    
    if (*p==21) {        /* constant */
        c = p[3];
        switch(cvn) {

        case 99:        /* c18 */
            c <<= 1;

        case 98:        /* c14 */
            c <<= 1;

        case 97:        /* c12 */
            c <<= 1;

            p[3] = c;
        return(p);
        }
    }
    return(block(1, cvn, t, max(1,d), p, NULL, NULL));
}

int chkw(int *p) {
    int t;

    if ((t=p[1])>1 && t<16)
        error("Integer operand required");
    return 0;
}

int lintyp(int t) {
    return(t<16? t:(t<32? t-12: 8));
}

int *block(int n, int op, int t, int d, int *p1, int *p2, int *p3) {
    int *p;

    printf("DEBUG: block() called with n=%d, op=%d\n", n, op);
    
    p = space;
    n += 3;
    
    if(space+n >= ospace+ossiz) {
        error("Expression overflow");
        exit(1);
    }
    
    /* Store the parameters in the space */
    *space++ = op;
    *space++ = t;
    *space++ = d;
    if (p1) *space++ = (long)p1;  /* Store as long to avoid truncation */
    if (p2) *space++ = (long)p2;
    if (p3) *space++ = (long)p3;
    
    return(p);
}

void chklval(int *p) {
    if (*p!=20)
        if (*p!=36)
            error("Lvalue required");
}

int notcompat(int at, int st) {
    if (st==0)        /* word, byte */
        return(at>1 && at<16);
    if (st==1)        /* word */
        return(at>0 && at<16);
    return((st-2) != at);
}

int max(int a, int b) {
    if (a>b)
        return(a);
    return(b);
}