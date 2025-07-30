/* C compiler - c03.c Code Generation
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernized for contemporary C compilers - Archaeological restoration
 * PDP-11 Assembly Code Generator
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

/* External globals from c00.c */
extern int *space;
extern int ossiz;
extern int line;
extern int stack;
extern int *csym;
extern int namsiz;
extern int ncpw;
extern int fout;
extern char *tmpfil;  /* Added tmpfil */

/* External functions */
extern int easystmt(void);
extern int *block();

/* Forward declarations */
void jumpc(int *tree, int lbl, int cond);
void rcexpr(int *tree, int table);
void jump(int lab);
void label(int l);
void retseq(void);
void slabel(void);
void setstk(int a);
void defvec(int a);
void defstat(int *s);
int length(int t);
int rlength(int c);
void printn(int n, int b);
int cprintf(char *fmt, ...);  /* Renamed to avoid conflict */
void putwrd(int w);
void cputchar(int c);         /* Renamed to avoid conflict */

/* Custom putchar for compiler output */
void cputchar(int c) {
    write(fout, &c, 1);
}

void jumpc(int *tree, int lbl, int cond) {
    /* Generate conditional jump based on tree evaluation */
    rcexpr((int*)block(1, easystmt()+103, (intptr_t)tree, lbl, cond), 2); /* cctab */
}

void rcexpr(int *tree, int table) {
    static FILE *temp_file = NULL;
    int c, *sp;
    int *ospace = space; /* Start of expression space */

    /* Open temp file on first call */
    if (!temp_file) {
        temp_file = fopen(tmpfil, "a");  /* Append mode */
        if (!temp_file) {
            printf("Error: Cannot open temp file %s\n", tmpfil);
            exit(1);
        }
    }

    /* Write the # marker at the beginning */
    fputc('#', temp_file);
    
    c = (space - ospace) / 2;  /* # addresses per word */
    sp = ospace;

    /* Write intermediate code in the correct format */
    fprintf(temp_file, "%04x %04x %04x %04x", 
            c & 0xFFFF, 
            (int)(intptr_t)tree & 0xFFFF, 
            table & 0xFFFF, 
            line & 0xFFFF);
    
    /* Write additional expression data */
    while(c--) {
        fprintf(temp_file, " %04x", *sp++ & 0xFFFF);
    }
        
    fputc('\n', temp_file);  /* End the intermediate code block */
    fflush(temp_file);
}

void jump(int lab) {
    cprintf("jmp\tL%d\n", lab);
}

void label(int l) {
    cprintf("L%d:", l);
}

void retseq(void) {
    cprintf("jmp\tretrn\n");
}

void slabel(void) {
    cprintf(".data; L%d: 1f; .text; 1:\n", csym[2]);
}

void setstk(int a) {
    int ts;

    ts = a - stack;
    stack = a;
    switch(ts) {

    case 0:
        return;

    case -2:  /* 0177776 in octal */
        cprintf("tst\t-(sp)\n");
        return;

    case -4:  /* 0177774 in octal */
        cprintf("cmp\t-(sp),-(sp)\n");
        return;
    }
    cprintf("add\t$%o,sp\n", ts & 0177777); /* Ensure octal output */
}

void defvec(int a) {
    cprintf("mov\tsp,r0\nmov\tr0,-(sp)\n");
    stack -= 2;
}

void defstat(int *s) {
    int len;

    len = length(s[1]);
    if (s[3])
        cprintf(".data; L%d:1f; .bss; 1:.=.+%o; .even; .text\n", s[2],
            s[3]*len);
    else
        cprintf(".bss; L%d:.=.+%o; .even; .text\n", s[2], len);
}

int length(int t) {
    if (t<0)
        t += 020;
    if (t>=020)
        return(2);
    switch(t) {

    case 0:
        return(2);  /* int */

    case 1:
        return(1);  /* char */

    case 2:
        return(4);  /* float */

    case 3:
        return(8);  /* double */

    case 4:
        return(4);  /* long */

    }
    return(1024);
}

int rlength(int c) {
    int l;

    return((l=length(c))==1? 2: l);
}

void printn(int n, int b) {
    int a;

    if(a=n/b) /* assignment, not test for equality */
        printn(a, b); /* recursive */
    cputchar(n%b + '0');
}

/* Custom printf implementation for the compiler */
int cprintf(char *fmt, ...) {
    va_list args;
    char *s;
    int x, c;

    va_start(args, fmt);

loop:
    while((c = *fmt++) != '%') {
        if(c == '\0') {
            va_end(args);
            return 0;
        }
        cputchar(c);
    }
    x = va_arg(args, int);
    switch (c = *fmt++) {

    case 'd': /* decimal */
    case 'o': /* octal */
        if(x < 0) {
            x = -x;
            if(x<0)  {    /* - infinity */
                if(c=='o')
                    cprintf("100000");
                else
                    cprintf("-32767");
                goto loop;
            }
            cputchar('-');
        }
        printn(x, c=='o'?8:10);
        goto loop;

    case 's': /* string */
        s = (char*)(intptr_t)x;  /* Fixed cast */
        while((c = *s++))
            cputchar(c);
        goto loop;

    case 'p': /* symbol name */
        s = (char*)(intptr_t)x;  /* Fixed cast */
        cputchar('_');
        c = namsiz;
        while(c--)
            if(*s)
                cputchar(*s++);
        goto loop;
    }
    cputchar('%');
    fmt--;
    /* Note: original decremented adx here, but we can't with va_list */
    goto loop;
}

/* Word output for intermediate code - now unused since rcexpr writes directly */
void putwrd(int w) {
    /* This function is now unused - rcexpr handles writing directly */
}