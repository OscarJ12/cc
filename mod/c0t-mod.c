/*
 * C compiler - Operator Tables and File I/O
 * 
 * Originally: Copyright 1972 Bell Telephone Laboratories, Inc.
 * Modernized for contemporary C compilers - Archaeological restoration
 * 
 * Contains the operator precedence tables, type conversion tables,
 * and file I/O functions for the second pass code generator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/* External globals */
extern char *tmpfil;
extern int nerror;

/* File buffer for word I/O */
static FILE *buf = NULL;

/* Word output function */
void putwrd(int w) {
    if (!buf) {
        buf = fopen(tmpfil, "w");
        if (!buf) {
            write(2, "Temp file botch.\n", 17);
            exit(1);
        }
    }
    fprintf(buf, "%04x ", w & 0xFFFF);
}

/* Word input function - reads hex words from intermediate file */
int getwrd(void) {
    static char buffer[1024];
    static int pos = 0;
    static int len = 0;
    int result = 0;
    int c;
    extern int fin;  /* Input file descriptor from c10-mod.c */
    
    /* Skip whitespace */
    while (1) {
        if (pos >= len) {
            len = read(fin, buffer, sizeof(buffer));
            pos = 0;
            if (len <= 0) return 0;
        }
        c = buffer[pos++];
        if (c != ' ' && c != '\t' && c != '\n') break;
    }
    
    /* Read hex number */
    pos--;  /* Back up one */
    while (pos < len) {
        c = buffer[pos++];
        if (c >= '0' && c <= '9')
            result = result * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f')
            result = result * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            result = result * 16 + (c - 'A' + 10);
        else {
            pos--;  /* Back up */
            break;
        }
    }
    
    return result;
}

/* Flush word output */
void flshw(void) {
    if (buf) {
        fflush(buf);
    }
}

/* Operator precedence and properties table */
int opdope[] = {
    00000,    /* 0: EOF */
    00000,    /* 1: ; */
    00000,    /* 2: { */
    00000,    /* 3: } */
    036000,   /* 4: [ */
    02000,    /* 5: ] */
    036000,   /* 6: ( */
    02000,    /* 7: ) */
    014201,   /* 8: : */
    07001,    /* 9: , */
    00000,    /* 10 */
    00000,    /* 11 */
    00000,    /* 12 */
    00000,    /* 13 */
    00000,    /* 14 */
    00000,    /* 15 */
    00000,    /* 16 */
    00000,    /* 17 */
    00000,    /* 18 */
    00000,    /* 19 */
    00000,    /* 20: name */
    00000,    /* 21: short constant */
    00000,    /* 22: string */
    00000,    /* 23: float */
    00000,    /* 24: double */
    00000,    /* 25 */
    00000,    /* 26 */
    00000,    /* 27 */
    00000,    /* 28 */
    00000,    /* 29 */
    034202,   /* 30: ++pre */
    034202,   /* 31: --pre */
    034202,   /* 32: ++post */
    034202,   /* 33: --post */
    034220,   /* 34: !un */
    034202,   /* 35: &un */
    034220,   /* 36: *un */
    034200,   /* 37: -un */
    034220,   /* 38: ~un */
    00000,    /* 39 */
    030101,   /* 40: + */
    030001,   /* 41: - */
    032101,   /* 42: * */
    032001,   /* 43: / */
    032001,   /* 44: % */
    026061,   /* 45: >> */
    026061,   /* 46: << */
    020161,   /* 47: & */
    016161,   /* 48: | */
    016161,   /* 49: ^ */
    00000,    /* 50 */
    00000,    /* 51 */
    00000,    /* 52 */
    00000,    /* 53 */
    00000,    /* 54 */
    00000,    /* 55 */
    00000,    /* 56 */
    00000,    /* 57 */
    00000,    /* 58 */
    00000,    /* 59 */
    022105,   /* 60: == */
    022105,   /* 61: != */
    024105,   /* 62: <= */
    024105,   /* 63: < */
    024105,   /* 64: >= */
    024105,   /* 65: > */
    024105,   /* 66: <p */
    024105,   /* 67: <=p */
    024105,   /* 68: >p */
    024105,   /* 69: >=p */
    012213,   /* 70: =+ */
    012213,   /* 71: =- */
    012213,   /* 72: =* */
    012213,   /* 73: =/ */
    012213,   /* 74: =% */
    012253,   /* 75: =>> */
    012253,   /* 76: =<< */
    012253,   /* 77: =& */
    012253,   /* 78: =| */
    012253,   /* 79: =^ */
    012213,   /* 80: = */
    00000,    /* 81 */
    00000,    /* 82 */
    00000,    /* 83 */
    00000,    /* 84: int -> float */
    00000,    /* 85: int -> double */
    00000,    /* 86: float -> int */
    00000,    /* 87: float -> double */
    00000,    /* 88: double -> int */
    00000,    /* 89: double -> float */
    014201,   /* 90: ? */
    00000,    /* 91 */
    00000,    /* 92 */
    00000,    /* 93 */
    00000,    /* 94: int -> float */
    00000,    /* 95: int -> double */
    00000,    /* 96: float -> double */
    00000,    /* 97: int -> int[] */
    00000,    /* 98: int -> float[] */
    00000,    /* 99: int -> double[] */
    036001,   /* 100: call */
    036001,   /* 101: mcall */
    0         /* End marker */
};

/* Type conversion table */
char cvtab[] = {
    /* i:i    i:c    i:f    i:d    i:i[]  i:c[]  i:f[]  i:d[]  i:[][] */
    000,    000,    0113,   0125,   0140,   0100,   0150,   0160,   0140,
    
    /* c:i    c:c    c:f    c:d    c:i[]  c:c[]  c:f[]  c:d[]  c:[][] */
    0100,   0100,   0113,   0125,   0140,   0100,   0150,   0160,   0140,
    
    /* f:i    f:c    f:f    f:d    f:i[]  f:c[]  f:f[]  f:d[]  f:[][] */
    0211,   0211,   000,    0136,   0211,   0211,   0211,   0211,   0211,
    
    /* d:i    d:c    d:f    d:d    d:i[]  d:c[]  d:f[]  d:d[]  d:[][] */
    0222,   0222,   0234,   000,    0222,   0222,   0222,   0222,   0222,
    
    /* i[]:i  i[]:c  i[]:f  i[]:d  i[]:i[] i[]:c[] i[]:f[] i[]:d[] i[]:[][] */
    0240,   0240,   0113,   0125,   000,    000,    0100,   0100,   0100,
    
    /* c[]:i  c[]:c  c[]:f  c[]:d  c[]:i[] c[]:c[] c[]:f[] c[]:d[] c[]:[][] */
    000,    000,    0113,   0125,   0200,   000,    0200,   0200,   0200,
    
    /* f[]:i  f[]:c  f[]:f  f[]:d  f[]:i[] f[]:c[] f[]:f[] f[]:d[] f[]:[][] */
    0250,   0250,   0113,   0125,   000,    000,    000,    0100,   000,
    
    /* d[]:i  d[]:c  d[]:f  d[]:d  d[]:i[] d[]:c[] d[]:f[] d[]:d[] d[]:[][] */
    0260,   0260,   0113,   0125,   000,    000,    000,    000,    000,
    
    /* [][]:i [][]:c [][]:f [][]:d [][]:i[] [][]:c[] [][]:f[] [][]:d[] [][]:[][] */
    0240,   0240,   0113,   0125,   000,    000,    0100,   0100,   000
};

/* Instruction selection table - using indices instead of pointers */
char *instruction_strings[] = {
    "add", "sub", "inc", "dec", "beq", "bne", "ble", "bgt", 
    "blt", "bge", "mov", "cmp", "tst", "jmp", "br"
};

/* Simplified instruction table - will be initialized at runtime */
int instab[100];

/* Initialize instruction table at runtime */
void init_instab(void) {
    int i = 0;
    /* + operator */
    instab[i++] = 40; instab[i++] = 0; instab[i++] = 0; /* add */
    instab[i++] = 70; instab[i++] = 0; instab[i++] = 0; /* += */
    instab[i++] = 41; instab[i++] = 1; instab[i++] = 1; /* - */
    instab[i++] = 71; instab[i++] = 1; instab[i++] = 1; /* -= */
    instab[i++] = 30; instab[i++] = 2; instab[i++] = 2; /* ++pre */
    instab[i++] = 31; instab[i++] = 3; instab[i++] = 3; /* --pre */
    instab[i++] = 32; instab[i++] = 2; instab[i++] = 2; /* ++post */
    instab[i++] = 33; instab[i++] = 3; instab[i++] = 3; /* --post */
    
    /* Comparison operators */
    instab[i++] = 60; instab[i++] = 4; instab[i++] = 5; /* == */
    instab[i++] = 61; instab[i++] = 5; instab[i++] = 4; /* != */
    instab[i++] = 62; instab[i++] = 6; instab[i++] = 7; /* <= */
    instab[i++] = 63; instab[i++] = 8; instab[i++] = 9; /* < */
    instab[i++] = 64; instab[i++] = 9; instab[i++] = 8; /* >= */
    instab[i++] = 65; instab[i++] = 7; instab[i++] = 6; /* > */
    
    instab[i] = 0;  /* End marker */
}

/* Character type table - fixed octal constant */
char ctab[] = {
    000, 0127, 0127, 0127, 0127, 0127, 0127, 0127,
    0127, 0126, 0125, 0127, 0127, 0127, 0127, 0127,
    0127, 0127, 0127, 0127, 0127, 0127, 0127, 0127,
    0127, 0127, 0127, 0127, 0127, 0127, 0127, 0127,
    0126, 034, 0122, 0127, 0127, 044, 047, 0121,
    006, 007, 042, 040, 011, 041, 0127, 043,
    0124, 0124, 0124, 0124, 0124, 0124, 0124, 0124,
    0124, 0124, 010, 001, 063, 0100, 065, 0110,
    0127, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 004, 0127, 005, 051, 0127, /* Fixed: 049 -> 051 */
    0127, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 0123, 0123, 0123, 0123, 0123,
    0123, 0123, 0123, 002, 050, 003, 0127, 0127
};

/* Template strings for code generation */
char *template_strings[] = {
    "HA", "A", "FA", "GA", "KA", "LA", "AB", "O", "GO", "HO", "ABO", 
    "#", "AH", "GH", ""
};

/* Register/table definitions for code generation - using indices */
int regtab[50];
int efftab[50]; 
int cctab[50];
int sptab[50];

/* Initialize code generation tables at runtime */
void init_tables(void) {
    int i;
    
    /* Initialize regtab */
    i = 0;
    regtab[i++] = 20; regtab[i++] = 0; regtab[i++] = 0;  /* name -> register (HA) */
    regtab[i++] = 21; regtab[i++] = 1; regtab[i++] = 0;  /* constant -> register (A) */
    regtab[i++] = 40; regtab[i++] = 2; regtab[i++] = 0;  /* + operation (FA) */
    regtab[i++] = 41; regtab[i++] = 3; regtab[i++] = 0;  /* - operation (GA) */
    regtab[i++] = 42; regtab[i++] = 4; regtab[i++] = 0;  /* * operation (KA) */
    regtab[i++] = 43; regtab[i++] = 5; regtab[i++] = 0;  /* / operation (LA) */
    regtab[i++] = 80; regtab[i++] = 6; regtab[i++] = 0;  /* = assignment (AB) */
    regtab[i] = 0;  /* End marker */
    
    /* Initialize efftab */
    i = 0;
    efftab[i++] = 20; efftab[i++] = 7; efftab[i++] = 0;  /* name (O) */
    efftab[i++] = 21; efftab[i++] = 7; efftab[i++] = 0;  /* constant (O) */
    efftab[i++] = 40; efftab[i++] = 8; efftab[i++] = 0;  /* + (GO) */
    efftab[i++] = 41; efftab[i++] = 9; efftab[i++] = 0;  /* - (HO) */
    efftab[i++] = 80; efftab[i++] = 10; efftab[i++] = 0; /* = (ABO) */
    efftab[i] = 0;  /* End marker */
    
    /* Initialize cctab */
    i = 0;
    cctab[i++] = 20; cctab[i++] = 1; cctab[i++] = 0;   /* name (A) */
    cctab[i++] = 21; cctab[i++] = 11; cctab[i++] = 0;  /* constant (#) */
    cctab[i++] = 60; cctab[i++] = 6; cctab[i++] = 0;   /* == (AB) */
    cctab[i++] = 61; cctab[i++] = 6; cctab[i++] = 1;   /* != (AB) */
    cctab[i] = 0;  /* End marker */
    
    /* Initialize sptab */
    i = 0;
    sptab[i++] = 20; sptab[i++] = 12; sptab[i++] = 0;  /* name -> stack (AH) */
    sptab[i++] = 21; sptab[i++] = 12; sptab[i++] = 0;  /* constant -> stack (AH) */
    sptab[i++] = 40; sptab[i++] = 13; sptab[i++] = 0;  /* + -> stack (GH) */
    sptab[i] = 0;  /* End marker */
}