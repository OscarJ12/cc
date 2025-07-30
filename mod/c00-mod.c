/* C compiler - 1972 Bell Labs C Compiler
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernised for contemporary systems - core functionality only
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

/* Global variables - from 1972 storage section */
int ossiz = 250;
int regtab = 0;
int efftab = 1;
int cctab = 2;
int sptab = 3;
char symbuf[32];
int pssiz = 8;
int namsiz = 8;
int nwps = 4;
int hshused = 0;
int hshsiz = 100;
int hshlen = 800;
int hshtab[800];
int *space = NULL;
int **cp = NULL;
int cmsiz = 40;
int cmst[40];
int ctyp = 0;
int isn = 1;
int swsiz = 120;
int swtab[120];
int *swp = 0;  /* Fixed: should be pointer */
int contlab = 0;
int brklab = 0;
int deflab = 0;
int nreg = 4;
int maprel[] = {60,61,64,65,62,63,68,69,66,67};
int nauto = 0;
int stack = 0;
int peeksym = -1;
int peekc = 0;
int eof = 0;
int line = 1;
int *csym = 0;
int cval = 0;
int ncpw = 2;
int nerror = 0;
int *paraml = 0;
int *parame = 0;
char *tmpfil = 0;
int fin = 0;
int fout = 1;

/* Character classification table - from original assembly */
unsigned char ctab[256] = {
    0,127,127,127,127,127,127,127, 127,126,125,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    126,34,122,127,127,44,47,121, 6,7,42,40,9,41,127,43,
    124,124,124,124,124,124,124,124, 124,124,8,1,63,80,65,90,
    127,123,123,123,123,123,123,123, 123,123,123,123,123,123,123,123,
    123,123,123,123,123,123,123,123, 123,123,123,4,127,5,49,127,
    127,123,123,123,123,123,123,123, 123,123,123,123,123,123,123,123,
    123,123,123,123,123,123,123,123, 123,123,123,2,48,3,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127, 127,127,127,127,127,127,127,127
};

/* Forward declarations - functions defined in this module */
int *lookup();
int symbol();
int subseq(int c, int a, int b);
int getstr();
int getcc();
int mapch(int c);
void error(char *s);
void flush();
void flshw();
int mygetchar();

/* Forward declarations - functions defined in c02-mod.c */
extern void extdef(void);
extern void blkend(void);

/* Function prototypes for c01.c integration */
extern int *build(int op);
/* Original block() function signature from 1972 - variable arguments */
extern int *block();

void ospace() {}

int init(char *s, int t) {
    int *np, i;

    i = namsiz;
    memset(symbuf, 0, sizeof(symbuf));
    strncpy(symbuf, s, namsiz-1);
    
    np = lookup();
    *np = 1;      /* Mark as keyword */
    *(np+1) = t;  /* Store type value */
    return 0;
}

int main(int argc, char *argv[]) {
    int i;

    if(argc<4) {
        printf("Usage: %s input.c output.s temp.tmp\n", argv[0]);
        printf("1972 C Compiler - Lexical Analyzer\n");
        exit(1);
    }
    
    if((fin=open(argv[1], O_RDONLY))<0) {
        error("Can't find input file");
        exit(1);
    }
    if((fout=open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644))<0) {
        error("Can't create output file");
        exit(1);
    }
    
    tmpfil = argv[3];
    
    /* Initialise hash table */
    for(i=0; i<800; i++) hshtab[i] = 0;
    
    /* Allocate expression space - CRITICAL FIX */
    space = malloc(ossiz * sizeof(int));
    if (!space) {
        error("Cannot allocate expression space");
        exit(1);
    }
    
    /* Initialise keyword table */
    init("int", 0);
    init("char", 1);
    init("float", 2);
    init("double", 3);
    init("auto", 5);
    init("extern", 6);
    init("static", 7);
    init("goto", 10);
    init("return", 11);
    init("if", 12);
    init("while", 13);
    init("else", 14);
    init("switch", 15);
    init("case", 16);
    init("break", 17);
    init("continue", 18);
    init("do", 19);
    init("default", 20);
    
    /* Main compilation loop - calls functions defined in c02-mod.c */
    while(!eof) {
        extdef();   /* Defined in c02-mod.c */
        blkend();   /* Defined in c02-mod.c */
    }
    
    flush();
    flshw();
    exit(nerror!=0);
}

int *lookup() {
    int i, j;
    int *np, *sp, *rp;

    i = 0;
    sp = (int*)symbuf;
    j = nwps;
    while(j--)
        i += *sp++;
    if (i<0) i = -i;
    i %= hshsiz;
    i *= pssiz;
    
    while(*(np = &hshtab[i+4])) {
        sp = (int*)symbuf;
        j = nwps;
        while(j--)
            if (*np++ != *sp++) goto no;
        return(&hshtab[i]);
no:        
        if ((i += pssiz) >= hshlen) i = 0;
    }
    
    if(hshused++ > hshsiz) {
        error("Symbol table overflow");
        exit(1);
    }
    
    rp = np = &hshtab[i];
    sp = (int*)symbuf;
    j = 4;
    while(j--)
        *np++ = 0;
    j = nwps;
    while(j--)
        *np++ = *sp++;
    
    return(rp);
}

int symbol() {
    int b, c;
    char *sp;

    if (peeksym>=0) {
        c = peeksym;
        peeksym = -1;
        return(c);
    }
    
    if (peekc) {
        c = peekc;
        peekc = 0;
    } else {
        if (eof)
            return(0);
        else
            c = mygetchar();
    }
    
loop:
    if (c < 0 || c > 255) {
        c = mygetchar();
        goto loop;
    }
        
    switch(ctab[c]) {

    case 125:    /* newline */
        line++;

    case 126:    /* white space */
        c = mygetchar();
        goto loop;

    case 0:        /* EOF */
        eof++;
        return(0);

    case 40:    /* + */
        return(subseq(c,40,30));

    case 41:    /* - */
        return(subseq(c,41,31));

    case 80:    /* = */
        if (subseq(' ',0,1)) return(80);
        c = symbol();
        if (c>=40 && c<=49)
            return(c+30);
        if (c==80)
            return(60);
        peeksym = c;
        return(80);

    case 63:    /* < */
        if (subseq(c,0,1)) return(46);
        return(subseq('=',63,62));

    case 65:    /* > */
        if (subseq(c,0,1)) return(45);
        return(subseq('=',65,64));

    case 34:    /* ! */
        return(subseq('=',34,61));

    case 43:    /* / */
        if (subseq('*',1,0))
            return(43);
        /* Comment handling */
    com:
        c = mygetchar();
    com1:
        if (c=='\0') {
            eof++;
            error("Nonterminated comment");
            return(0);
        }
        if (c=='\n')
            line++;
        if (c!='*')
            goto com;
        c = mygetchar();
        if (c!='/')
            goto com1;
        c = mygetchar();
        goto loop;

    case 124:    /* number */
        cval = 0;
        if (c=='0')
            b = 8;
        else
            b = 10;
        while(ctab[c]==124) {
            cval = cval*b + c -'0';
            c = mygetchar();
        }
        peekc = c;
        return(21);

    case 122:    /* " */
        return(getstr());

    case 121:    /* ' */
        return(getcc());

    case 123:    /* letter */
        sp = symbuf;
        while(ctab[c]==123 || ctab[c]==124) {
            if (sp<symbuf+namsiz) *sp++ = c;
            c = mygetchar();
        }
        while(sp<symbuf+namsiz)
            *sp++ = '\0';
        peekc = c;
        csym = lookup();
        if (csym[0]==1) {    /* keyword */
            cval = csym[1];
            return(19);
        }
        return(20);

    case 127:    /* unknown */
        error("Unknown character");
        c = mygetchar();
        goto loop;
    }
    
    return(ctab[c]);
}

int subseq(int c, int a, int b) {
    if (!peekc)
        peekc = mygetchar();
    if (peekc != c)
        return(a);
    peekc = 0;
    return(b);
}

int getstr() {
    int c;

    printf(".data;L%d:.byte ", cval=isn++);
    while((c=mapch('"')) >= 0)
        printf("%o,", c);
    printf("0;.even;.text\n");
    return(22);
}

int getcc() {
    int c, cc;
    char *cp;

    cval = 0;
    cp = (char*)&cval;
    cc = 0;
    while((c=mapch('\'')) >= 0)
        if(cc++ < ncpw)
            *cp++ = c;
    if(cc>ncpw)
        error("Long character constant");
    return(21);
}

int mapch(int c) {
    int a;

    if((a=mygetchar())==c)
        return(-1);
    
    switch(a) {
    case '\n':
    case 0:
        error("Nonterminated string");
        peekc = a;
        return(-1);

    case '\\':
        switch (a=mygetchar()) {
        case 't':
            return('\t');
        case 'n':
            return('\n');
        case '0':
            return('\0');
        case 'r':
            return('\r');
        case '\n':
            line++;
            return('\n');
        }
    }
    
    return(a);
}

int mygetchar() {
    static char buffer[1024];
    static int pos = 0;
    static int len = 0;
    
    if (pos >= len) {
        len = read(fin, buffer, sizeof(buffer));
        pos = 0;
        if (len <= 0) {
            eof = 1;
            return 0;
        }
    }
    
    return (unsigned char)buffer[pos++];
}

void error(char *s) {
    printf("Error: %s\n", s);
    nerror++;
}

/* Complete original tree() function from c00.c - full 1972 implementation */
int *tree(void) {
    int op[20], *opp, *pp, prst[20], andflg, o;
    int p, ps, os;

    /* Initialize expression parsing - like original */
    opp = op;           /* operator stack pointer */
    pp = prst;          /* precedence stack pointer */
    cp = (int**)cmst;
    *opp = 200;         /* stack EOF */
    *pp = 6;            /* precedence */
    andflg = 0;

advanc:
    switch (o=symbol()) {

    /* name */
    case 20:
        if (*csym==0) {
            if((peeksym=symbol())==6)
                *csym = 6;    /* extern */
            else {
                if(csym[2]==0)    /* unseen so far */
                    csym[2] = isn++;
            }
        }
        if(*csym==6)    /* extern */
            *(int**)cp++ = (int*)block(5,20,csym[1],0,*csym,csym[4],csym[5],csym[6],csym[7]);
        else
            *(int**)cp++ = (int*)block(2,20,csym[1],0,*csym,csym[2]);
        goto tand;

    /* short constant */
    case 21:
    case21:
        *(int**)cp++ = (int*)block(1,21,ctyp,0,cval);
        goto tand;

    /* string constant */
    case 22:
        *(int**)cp++ = (int*)block(1,22,17,0,cval);

tand:
        if((int**)cp >= (int**)(cmst+cmsiz)) {
            error("Expression overflow");
            exit(1);
        }
        if (andflg)
            goto syntax;
        andflg = 1;
        goto advanc;

    /* ++, -- */
    case 30:
    case 31:
        if (andflg)
            o += 2;
        goto oponst;

    /* ! */
    case 34:
        if (andflg)
            goto syntax;
        goto oponst;

    /* - */
    case 41:
        if (!andflg) {
            peeksym = symbol();
            if (peeksym==21) {
                peeksym = -1;
                cval = -cval;
                goto case21;
            }
            o = 37;
        }
        andflg = 0;
        goto oponst;

    /* & */
    /* * */
    case 47:
    case 42:
        if (andflg)
            andflg = 0; 
        else
            if(o==47)
                o = 35;
            else
                o = 36;
        goto oponst;

    /* ( */
    case 6:
        if (andflg) {
            o = symbol();
            if (o==7)
                o = 101; 
            else {
                peeksym = o;
                o = 100;
                andflg = 0;
            }
        }
        goto oponst;

    /* ) */
    /* ] */
    case 5:
    case 7:
        if (!andflg)
            goto syntax;
        goto oponst;
    }

    /* binaries - all other operators */
    if (!andflg)
        goto syntax;
    andflg = 0;

oponst:
    /* Get operator precedence - simplified but functional */
    switch(o) {
    case 80: p = 8; break;    /* = */
    case 9:  p = 1; break;    /* , */
    case 40: p = 12; break;   /* + */
    case 41: p = 12; break;   /* - */
    case 42: p = 13; break;   /* * */
    case 43: p = 13; break;   /* / */
    case 60: p = 7; break;    /* == */
    case 61: p = 7; break;    /* != */
    case 62: p = 9; break;    /* <= */
    case 63: p = 9; break;    /* < */
    case 64: p = 9; break;    /* >= */
    case 65: p = 9; break;    /* > */
    case 34: p = 15; break;   /* ! */
    case 35: p = 15; break;   /* & unary */
    case 36: p = 15; break;   /* * unary */
    case 37: p = 15; break;   /* - unary */
    default: p = 6; break;    /* default */
    }

opon1:
    ps = *pp;
    if (p > ps || (p == ps && (o == 80))) { /* = is right associative */
putin:
        switch (o) {
        case 6: /* ( */
        case 4: /* [ */
        case 100: /* call */
            p = 4;
        }
        if(opp >= op+19) {        /* opstack size check */
            error("Expression overflow");
            exit(1);
        }
        *++opp = o;      /* push operator */
        *++pp = p;       /* push precedence */
        goto advanc;
    }
    
    /* Pop and process operator */
    --pp;
    switch (os = *opp--) {

    /* EOF */
    case 200:
        peeksym = o;
        return(*(int**)--cp);

    /* call */
    case 100:
        if (o!=7)
            goto syntax;
        build(os);
        goto advanc;

    /* mcall - method call */
    case 101:
        *(int**)cp++ = 0;        /* 0 arg call */
        os = 100;
        goto fbuild;

    /* ( */
    case 6:
        if (o!=7)
            goto syntax;
        goto advanc;

    /* [ */
    case 4:
        if (o!=5)
            goto syntax;
        build(4);
        goto advanc;
    }
    
fbuild:
    build(os);
    goto opon1;

syntax:
    error("Expression syntax");
    return(NULL);
}

/* Original declare() function from c00.c - modernized */
int declare(int kw) {
    int o;

    while((o=symbol())==20) {        /* name */
        if(kw>=5) {            /* type or sort? */
            if(*csym>0)
                error("Symbol redeclared");
            *csym = kw;
        } else {
            if ((csym[1]&017)!=0)
                error("Symbol redeclared");
            csym[1] |= (csym[1]&0760) | kw;
            if (*csym==0)
                *csym = -2;
        }
        while((o=symbol())==4) {    /* [ */
            if((o=symbol())==21) {    /* const */
                if(csym[1]>=020)
                    error("Bad vector");
                csym[3] = cval;
                o = symbol();
            }
            if (o!=5)        /* ] */
                goto syntax;
            csym[1] += 020;
        }
        if(kw==8)  {        /* parameter */
            *csym = -1;
            if (paraml==0)
                paraml = csym;
            else
                *parame = (intptr_t)csym; /* Fix pointer conversion */
            parame = csym;
        }
        if (o!=9)    /* , */
            break;
    }
    if((o==1 && kw!=8) || (o==7 && kw==8))
        return 0;
syntax:
    error("Declaration syntax");
    return 0;
}

void flush() {}
void flshw() {}
