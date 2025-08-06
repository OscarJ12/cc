/*
 * C compiler, part 2 - Second Pass Code Generator
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernized for contemporary C compilers - Archaeological restoration
 * 
 * This is the second pass that reads intermediate code and generates
 * actual PDP-11 assembly instructions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>

/* External global variables defined in c00-mod.c */
extern int nreg;
extern int isn;
extern int namsiz;
extern int line;
extern char *tmpfil;
extern int nerror;
extern int fltmod;
extern int fin, fout;

/* External functions from regtab-mod.c */
extern int generate_regtab_code(int opcode, int *operands, int op_count, int reg);
extern void init_real_regtab(void);

/* External tables - will be defined in c0t-mod.c */
extern int opdope[];
extern char cvtab[];
extern int instab[];
extern int cctab[], regtab[], efftab[], sptab[];

/* Initialization functions from c0t-mod.c */
extern void init_tables(void);

/* Forward declarations */
extern int getwrd(void);  /* Defined in c0t-mod.c */
void rcexpr(int *tree, int *table, int reg);
int cexpr(int **tree, int *table, int reg);
int match(int *tree, int *table, int nreg);
int dcalc(int *p, int nreg);
int notcompat(int at, int st);
void pname(int ***p);
void prins(int op, int c);
int collcon(int *p);
int isfloat(int *t, char **s);
void jumpc(int *tree, int lbl, int cond);
void cbranch(int *tree, int lbl, int cond, int reg);
void branch(int lbl, int op, int c);
void jump(int lab);
void label(int l);
void popstk(int a);
int length(int t);
int rlength(int c);
void printn(int n, int b);
int cprintf(char *fmt, ...);
void cputchar(int c);
void error(char *s, ...);

/* Fake functions from original */
void ospace(void) {}

void waste(void) {
    /* Original recursive waste function - simplified */
}

/* Main function is in c00-mod.c - this module provides code generation functions */

int match(int *tree, int *table, int nreg_param) {
    int op, d1, d2, t1, t2, *p1, *p2;
    char *mp;

    if (tree == 0)
        return 0;
        
    op = *tree;
    if (op >= 29)        /* if not leaf */
        p1 = (int*)tree[3];
    else
        p1 = tree;
        
    t1 = p1[1];
    d1 = dcalc(p1, nreg_param);
    
    if ((opdope[op] & 01) != 0) {    /* binary? */
        p2 = (int*)tree[4];
        t2 = p2[1];
        d2 = dcalc(p2, nreg_param);
    }
    
    while (*table) {
        if (*table++ == op) goto foundop;
        table++;
    }
    return 0;
    
foundop:
    table = (int*)*table;
    
nxtry:
    mp = (char*)table;
    if (*mp == 0)
        return 0;
        
    if (d1 > (*mp & 077) | ((*mp >= 0100) & (*p1 != 36)))
        goto notyet;
        
    if (notcompat(t1, mp[1]))
        goto notyet;
        
    if ((opdope[op] & 01) != 0 && p2 != 0) {
        if (d2 > (mp[2] & 077) | ((mp[2] >= 0100) & (*p2 != 36)))
            goto notyet;
        if (notcompat(t2, mp[3]))
            goto notyet;
    }
    
    return table[2];
    
notyet:
    table = table + 3;
    goto nxtry;
}

void rcexpr(int *tree, int *table, int reg) {
    if (tree == 0)
        return;
        
    if (*tree >= 103) {
        if (*tree == 103)
            jumpc((int*)tree[1], tree[2], tree[3]);
        else
            cbranch((int*)tree[1], tree[2], tree[3], 0);
        return;
    }
    
    /* Use real 1972 code generation if table is regtab */
    if (table == regtab) {
        int operands[5] = {0, 0, 0, 0, 0};
        
        /* Extract operands from tree - simplified for now */
        if (tree[3]) operands[0] = tree[3];  /* First operand */
        if (tree[4]) operands[1] = tree[4];  /* Second operand */
        
        cprintf("; Generating code for opcode %d\n", *tree);
        
        if (generate_regtab_code(*tree, operands, 2, reg)) {
            return;  /* Successfully generated with real templates */
        }
    }
    
    /* Fallback to simplified code generation */
    if (cexpr((int**)tree, table, reg))
        return;
        
    if (table != regtab) {
        if (cexpr((int**)tree, regtab, reg)) {
            if (table == sptab)
                cprintf("mov\tr%d,-(sp)\n", reg);
            if (table == cctab)
                cprintf("tst\tr%d\n", reg);
            return;
        }
    }
    
    error("No match for op %d", *tree);
}

int cexpr(int **tree, int *table, int reg) {
    int *p1, *p2, c, r, *p, *ctable;
    char *string, *match_result;
    int otable[10], regtab_local[10], cctab_local[10];

    if ((c = **tree) == 100) {        /* call */
        p1 = (int*)(*tree)[3];
        p2 = (int*)(*tree)[4];
        r = 0;
        
        if (p2) {
            while (*p2 == 9) {    /* comma */
                rcexpr((int*)p2[4], sptab, 0);
                r += rlength(((int*)p2[4])[1]);
                p2 = (int*)p2[3];
            }
            rcexpr(p2, sptab, 0);
            r += rlength(p2[1]);
        }
        
        **tree = 101;
        (*tree)[2] = r;        /* save arg length */
    }
    
    if (c == 90) {        /* ? */
        cbranch((int*)(*tree)[3], c = isn++, 0, reg);
        rcexpr((int*)(((int*)(*tree)[4])[3]), table, reg);
        branch(r = isn++, 0, 0);
        label(c);
        rcexpr((int*)(((int*)(*tree)[4])[4]), table, reg);
        label(r);
        return 1;
    }
    
    if ((string = (char*)match(*tree, table, nreg - reg)) == 0)
        return 0;
        
    p1 = (int*)(*tree)[3];
    p2 = (int*)(*tree)[4];
    
loop:
    switch (c = *string++) {
    
    case '\0':
        p = *tree;
        if (*p == 101 && p[2] > 0) {
            popstk(p[2]);
        }
        return 1;
        
    /* A1 */
    case 'A':
        p = (int*)(*tree)[3];
        goto adr;
        
    /* A2 */
    case 'B':
        p = (int*)(*tree)[4];
        goto adr;
        
    /* A */
    case 'O':
        p = *tree;
adr:
        pname((int***)&p);
        goto loop;
        
    /* I */
    case 'M':
        if ((c = *string) == '\'')
            string++;
        else
            c = 0;
        prins(**tree, c);
        goto loop;
        
    /* Other cases simplified for now */
    default:
        cputchar(c);
        goto loop;
    }
}

/* Simplified implementations of remaining functions */
int dcalc(int *p, int nreg_param) {
    int op;
    
    if (p == 0)
        return 0;
        
    op = *p;
    switch (op) {
    case 20:    /* name */
    case 22:    /* string */
    case 23:    /* float */
    case 24:    /* double */
        return 12;
        
    case 21:    /* short constant */
        return (p[3] == 0 ? 4 : 8);
        
    case 35:    /* & */
        return 12;
        
    case 36:    /* * */
        if ((op = dcalc((int*)p[3], nreg_param)) < 16)
            return 16;
    }
    
    return (p[2] <= nreg_param ? 20 : 24);
}

int notcompat(int at, int st) {
    if (st == 0)        /* word, byte */
        return (at > 1 && at < 16);
    if (st == 1)        /* word */
        return (at > 0 && at < 16);
    st -= 2;
    if (st == 2 && at == 3)
        at = 2;
    return (st != at);
}

void pname(int ***p) {
    cprintf("DEBUG: pname called\n");
}

void prins(int op, int c) {
    cprintf("DEBUG: prins op=%d c=%d\n", op, c);
}

int collcon(int *p) {
    return 0;  /* Simplified */
}

int isfloat(int *t, char **s) {
    return 0;  /* Simplified */
}

/* Input character from file */
int getchar(void) {
    static char buffer[1024];
    static int pos = 0;
    static int len = 0;
    
    if (pos >= len) {
        len = read(fin, buffer, sizeof(buffer));
        pos = 0;
        if (len <= 0) return EOF;
    }
    
    return (unsigned char)buffer[pos++];
}

/* Output functions */
void cputchar(int c) {
    write(fout, &c, 1);
}

int cprintf(char *fmt, ...) {
    va_list args;
    char buffer[1024];
    int len;
    
    va_start(args, fmt);
    len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    write(fout, buffer, len);
    return len;
}

void printn(int n, int b) {
    int a;
    if ((a = n / b))
        printn(a, b);
    cputchar(n % b + '0');
}

void error(char *s, ...) {
    va_list args;
    char buffer[1024];
    int len;
    
    nerror++;
    va_start(args, s);
    len = vsnprintf(buffer, sizeof(buffer), s, args);
    va_end(args);
    
    write(2, buffer, len);  /* stderr */
    write(2, "\n", 1);
}

/* Stubs for missing functions that will be implemented */
void jumpc(int *tree, int lbl, int cond) {
    cprintf("DEBUG: jumpc lbl=%d cond=%d\n", lbl, cond);
}

void cbranch(int *tree, int lbl, int cond, int reg) {
    cprintf("DEBUG: cbranch lbl=%d cond=%d reg=%d\n", lbl, cond, reg);
}

void branch(int lbl, int op, int c) {
    cprintf("br\tL%d\n", lbl);
}

void jump(int lab) {
    cprintf("jmp\tL%d\n", lab);
}

void label(int l) {
    cprintf("L%d:\n", l);
}

void popstk(int a) {
    switch (a) {
    case 0:
        return;
    case 2:
        cprintf("tst\t(sp)+\n");
        return;
    case 4:
        cprintf("cmp\t(sp)+,(sp)+\n");
        return;
    }
    cprintf("add\t$%o,sp\n", a);
}

int length(int t) {
    if (t < 0)
        t += 020;
    if (t >= 020)
        return 2;
    switch (t) {
    case 0: return 2;  /* int */
    case 1: return 1;  /* char */
    case 2: return 4;  /* float */
    case 3: return 8;  /* double */
    case 4: return 4;  /* long */
    }
    return 1024;
}

int rlength(int c) {
    int l = length(c);
    return ((l == 1) ? 2 : l);
}