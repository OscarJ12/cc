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
int swp = 0;
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

/* Forward declarations */
int *lookup();
int symbol();
int subseq(int c, int a, int b);
int getstr();
int getcc();
int mapch(int c);
void error(char *s);
void extdef();
void blkend();
void flush();
void flshw();
int mygetchar();

/* Function prototypes for c01.c integration */
extern int *build(int op);
extern int *block(int n, int op, int t, int d, int *p1, int *p2, int *p3);

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
    
    while(!eof) {
        extdef();
        blkend();
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

void extdef() {
    int token;
    int *expr_stack[10];
    
    /* Initialise expression parsing */
    cp = (int**)expr_stack;
    space = cmst;
    
    printf("Parsing: ");
    
    while((token = symbol()) != 0) {
        switch(token) {
            case 19: /* keyword */
                printf("%s ", symbuf);
                break;
            case 20: /* identifier */
                printf("%s ", symbuf);
                break;
            case 21: /* number */
                printf("%d ", cval);
                break;
            case 40: /* + */
                printf("+ ");
                break;
            case 41: /* - */
                printf("- ");
                break;
            case 42: /* * */
                printf("* ");
                break;
            case 43: /* / */
                printf("/ ");
                break;
            case 6: /* ( */
                printf("( ");
                break;
            case 7: /* ) */
                printf(") ");
                break;
            case 2: /* { */
                printf("{ ");
                break;
            case 3: /* } */
                printf("} ");
                break;
            case 1: /* ; */
                printf("; ");
                break;
            default:
                printf("[%d] ", token);
                break;
        }
    }
    
    printf("\n");
    eof = 1;
}

void blkend() {}
void flush() {}
void flshw() {}
