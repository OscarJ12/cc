/* C compiler - c02.c Statement Parser and Control Flow
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernized for contemporary C compilers - Archaeological restoration
 * Designed to link with c00-mod.c and c01-mod.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

/* Forward declarations */
extern int namsiz;
int function(void);
int statement(int d);
int *pexpr(void);
void pswitch(void);
void blkhed(void);
void errflush(int o);
void declist(void);
int easystmt(void);
void branch(int lab);
int declare(int kw);

/* External globals from c00.c */
extern int eof;
extern int cval;
extern int *csym;
extern int peeksym;
extern int peekc;
extern int line;
extern int nerror;
extern int isn;
extern int *paraml;
extern int *parame;
extern int contlab;
extern int brklab;
extern int deflab;
extern int *swp;
extern int swtab[];
extern int swsiz;
extern int hshtab[];
extern int hshsiz;
extern int pssiz;
extern int hshused;
extern int stack;

/* External functions from c00.c, c01.c, and c03.c */
extern int symbol(void);
extern void error(char *s, ...);
extern int *tree(void);
extern int *block();
extern int declare(int kw);

/* Code generation functions from c03-mod.c */
extern void rcexpr(int *tree_ptr, int table);
extern void retseq(void);
extern void jumpc(int *tree_ptr, int lab, int cond);
extern void jump(int lab);
extern void label(int l);
extern void slabel(void);
extern int length(int t);
extern int rlength(int c);
extern void setstk(int a);
extern void defvec(int a);
extern void defstat(int *s);



void extdef(void) {
    int o, c;
    int *cs;

    if(((o=symbol())==0) || o==1)    /* EOF or ; */
        return;
    if(o!=20)  /* Not an identifier */
        goto syntax;
        
    csym[0] = 6;  /* extern */
    cs = &csym[4];
    printf(".globl\t_");
    /* Print symbol name in PDP-11 assembly style */
    for (int i = 0; i < namsiz && csym[4 + i]; ++i) putchar(csym[4 + i]);
    putchar('\n');
    
    switch(o=symbol()) {

    case 6:    /* ( - function definition */
        printf("_"); for (int i = 0; i < namsiz && csym[4 + i]; ++i) putchar(csym[4 + i]); printf(":\n");
        function();
        return;

    case 21:   /* const - initialized variable */
        printf(".data; _");
        for (int i = 0; i < namsiz && csym[4 + i]; ++i) putchar(csym[4 + i]);
        printf(": %o\n", cval);
        if((o=symbol())!=1)    /* ; */
            goto syntax;
        return;

    case 1:    /* ; - uninitialized variable */
        printf(".bss; _");
        for (int i = 0; i < namsiz && csym[4 + i]; ++i) putchar(csym[4 + i]);
        printf(": .=.+2\n");
        return;

    case 4:    /* [ - array */
        c = 0;
        if((o=symbol())==21) {    /* const */
            c = cval<<1;
            o = symbol();
        }
        if(o!=5)    /* ] */
            goto syntax;
        printf("_"); for (int i = 0; i < namsiz && csym[4 + i]; ++i) putchar(csym[4 + i]); printf(":\n");
        if((o=symbol())==1) {    /* ; */
            printf(".bss; 1:.=.+%o\n", c);
            return;
        }
        printf("1:");
        while(o==21) {    /* const */
            printf("%o\n", cval);
            c -= 2;
            if((o=symbol())==1)    /* ; */
                goto done;
            if(o!=9)    /* , */
                goto syntax;
            else
                o = symbol();
        }
        goto syntax;
    done:
        if(c>0)
            printf(". = . + %o\n", c);
        return;

    case 0:    /* EOF */
        return;
    }

syntax:
    error("External definition syntax");
    errflush(o);
    statement(0);
}

int function(void) {
    printf("DEBUG: function() called\n");
    printf(".text\n");
    printf("mov\tr5,-(sp)\n");
    printf("mov\tsp,r5\n");
    
    declare(8);  /* Parameter declarations */
    declist();   /* Local declarations */
    statement(1); /* Function body */
    retseq();    /* Return sequence */
    
    return 0;
}

int statement(int d) {
    int o, o1, o2, o3;
    int *np;

stmt:
    switch(o=symbol()) {

    /* EOF */
    case 0:
        error("Unexpected EOF");
        return 0;
    /* ; */
    case 1:
    /* } */
    case 3:
        return 0;

    /* { */
    case 2:
        if(d)
            blkhed();
        while (!eof) {
            if ((o=symbol())==3)    /* } */
                goto bend;
            peeksym = o;
            statement(0);
        }
        error("Missing '}'");
    bend:
        return 0;

    /* keyword */
    case 19:
        switch(cval) {

        /* goto */
        case 10:
            o1 = isn++;  /* Label number */
            printf("DEBUG: goto statement to label %d\n", o1);
            goto semi;

        /* return */
        case 11:
            if((peeksym=symbol())==6) {   /* ( */
                rcexpr(pexpr(), 0);  /* regtab equivalent */
            }
            retseq();
            goto semi;

        /* if */
        case 12:
            o1 = isn++;  /* Label for end of if */
            jumpc(pexpr(), o1, 0);
            statement(0);  /* Then clause */
            if ((o=symbol())==19 && cval==14) {  /* else */
                o2 = isn++;
                if (easystmt()) {
                    branch(o2);
                } else {
                    jump(o2);
                }
                label(o1);
                statement(0);
                label(o2);
                return 0;
            }
            peeksym = o;
            label(o1);
            return 0;

        /* while */
        case 13:
            o1 = contlab;
            o2 = brklab;
            label(contlab = isn++);
            jumpc(pexpr(), brklab=isn++, 0);
            o3 = easystmt();
            statement(0);
            if (o3) {
                branch(contlab);
            } else {
                jump(contlab);
            }
            label(brklab);
            contlab = o1;
            brklab = o2;
            return 0;

        /* break */
        case 17:
            if(brklab==0)
                error("Nothing to break from");
            jump(brklab);
            goto semi;

        /* continue */
        case 18:
            if(contlab==0)
                error("Nothing to continue");
            jump(contlab);
            goto semi;

        /* do */
        case 19:
            o1 = contlab;
            o2 = brklab;
            contlab = isn++;
            brklab = isn++;
            label(o3 = isn++);
            statement(0);
            label(contlab);
            contlab = o1;
            if ((o=symbol())==19 && cval==13) { /* while */
                jumpc(tree(), o3, 1);
                label(brklab);
                brklab = o2;
                goto semi;
            }
            goto syntax;

        /* case */
        case 16:
            if ((o=symbol())!=21)    /* constant */
                goto syntax;
            if ((o=symbol())!=8)     /* : */
                goto syntax;
            if (swp==0) {
                error("Case not in switch");
                goto stmt;
            }
            if(swp>=swtab+swsiz) {
                error("Switch table overflow");
            } else {
                *swp++ = isn;
                *swp++ = cval;
                label(isn++);
            }
            goto stmt;

        /* switch */
        case 15:
            o1 = brklab;
            brklab = isn++;
            np = pexpr();
            if (np && np[1]>1 && np[1]<16)
                error("Integer required");
            if (np)
                rcexpr(np, 0);  /* regtab equivalent */
            pswitch();
            brklab = o1;
            return 0;

        /* default */
        case 20:
            if (swp==0)
                error("Default not in switch");
            if ((o=symbol())!=8)    /* : */
                goto syntax;
            deflab = isn++;
            label(deflab);
            goto stmt;
        }

        error("Unknown keyword");
        goto syntax;

    /* name */
    case 20:
        if (peekc==':') {
            peekc = 0;
            if (csym[0]>0) {
                error("Redefinition");
                goto stmt;
            }
            csym[0] = 2;
            csym[1] = 020;    /* int[] */
            if (csym[2]==0)
                csym[2] = isn++;
            slabel();
            goto stmt;
        }
        /* Fall through to expression */
    }

    peeksym = o;
    rcexpr(tree(), 1);  /* efftab equivalent */
    goto semi;

semi:
    if ((o=symbol())!=1)    /* ; */
        goto syntax;
    return 0;

syntax:
    error("Statement syntax");
    errflush(o);
    goto stmt;
}

int *pexpr(void) {
    int o;
    int *t;

    if ((o=symbol())!=6)    /* ( */
        goto syntax;
    t = tree();
    if ((o=symbol())!=7)    /* ) */
        goto syntax;
    return t;
syntax:
    error("Statement syntax");
    errflush(o);
    return NULL;
}

void pswitch(void) {
    int *sswp, dl, cv, swlab;

    sswp = swp;
    if (swp==0)
        swp = swtab;
    swlab = isn++;
    printf("jsr\tpc,bswitch; L%d\n", swlab);
    dl = deflab;
    deflab = 0;
    statement(0);
    if (!deflab) {
        deflab = isn++;
        label(deflab);
    }
    printf("L%d:.data;L%d:", brklab, swlab);
    while(swp>sswp && swp>swtab) {
        cv = *--swp;
        printf("%o; L%d\n", cv, *--swp);
    }
    printf("L%d; 0\n.text\n", deflab);
    deflab = dl;
    swp = sswp;
}

void blkend(void) {
    int i, hl;

    i = 0;
    hl = hshsiz;
    while(hl--) {
        if(hshtab[i+4]) {
            if (hshtab[i]==0)
                error("undefined symbol");
            if(hshtab[i]!=1) {    /* not keyword */
                hshused--;
                hshtab[i+4] = 0;
            }
        }
        i += pssiz;
    }
}

void errflush(int o) {
    while(o>3)    /* ; { } */
        o = symbol();
    peeksym = o;
}

void declist(void) {
    int o;

    while((o=symbol())==19 && cval<10)
        declare(cval);
    peeksym = o;
}

int easystmt(void) {
    if((peeksym=symbol())==20)    /* name */
        return(peekc!=':');       /* not label */
    if (peeksym==19) {            /* keyword */
        switch(cval) {
        case 10:    /* goto */
        case 11:    /* return */
        case 17:    /* break */
        case 18:    /* continue */
            return(1);
        }
        return(0);
    }
    return(peeksym!=2);    /* { */
}

void blkhed(void) {
    int o, al, pl;
    int *cs;
    int hl;

    printf("DEBUG: blkhed() - block header processing\n");
    declist();
    stack = al = -2;
    pl = 4;
    
    /* Process parameters */
    while(paraml) {
        *parame = 0;
        cs = paraml;
        paraml = (int*)(intptr_t)(*paraml);  /* Fix pointer conversion */
        cs[2] = pl;
        *cs = 10;
        pl += rlength(cs[1]);
    }
    
    /* Process local variables */
    cs = hshtab;
    hl = hshsiz;
    while(hl--) {
        if (cs[4]) {
            switch(cs[0]) {

            case -2:    /* unmentioned */
                cs[0] = 5;    /* auto */

            case 5:     /* auto */
                if (cs[3]) {    /* vector */
                    al -= (cs[3]*length(cs[1]-020)+1) & 0177776;
                    setstk(al);
                    defvec(al);
                }
                cs[2] = al;
                al -= rlength(cs[1]);
                break;

            case 10:    /* parameter */
                cs[0] = 5;
                break;

            case 7:     /* static */
                cs[2] = isn++;
                defstat(cs);
                break;
            }
        }
        cs += pssiz;
    }
    setstk(al);
}

void branch(int lab) {
    printf("br\tL%d\n", lab);
}
