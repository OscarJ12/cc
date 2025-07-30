/*
 * Real 1972 C Compiler Code Generation Templates
 * 
 * Originally: regtab.s from Bell Labs Unix V2 
 * Modernized for contemporary C compilers - Archaeological restoration
 * 
 * This contains the actual PDP-11 instruction templates that generate
 * assembly code from the intermediate representation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Template structure for code generation */
typedef struct {
    char *pattern;      /* Pattern like "%a,n" */
    char **code;        /* Array of instruction templates */
} CodeTemplate;

/* Forward declarations */
extern void cprintf(char *fmt, ...);

/* Instruction templates for each operator */

/* cr20 - name, constant */
char *cr20_templates[] = {
    "%z,n", "clr\tR", NULL,
    "%aw,n", "mov\tA,R", NULL,
    "%ab,n", "movb\tA,R", NULL,
    "%af,n", "M", "movf\tA,R", NULL,
    NULL
};

/* cr30 - ++,-- prefix */
char *cr30_templates[] = {
    "%ai,n", "%abp,n", "%ab,n", "IB1\tA1", "movB1\tA1,R", NULL,
    "%a,n", "I'\t$2,A1", "mov\tA1,R", NULL,
    "%nbp*,n", "%ni*,n", "%nb*,n", "F*", "IB1\t#1(R)", "movB1\t#1(R),R", NULL,
    "%n*,n", "F*", "I'\t$2,#1(R)", "mov\t#1(R),R", NULL,
    NULL
};

/* cr32 - ++,-- postfix */
char *cr32_templates[] = {
    "%ai,n", "%abp,n", "%ab,n", "movB1\tA1,R", "IB1\tA1", NULL,
    "%a,n", "mov\tA1,R", "I'\t$2,A1", NULL,
    "%nbp*,n", "%nb*,n", "%ni*,n", "F*", "movB1\t#1(R),-(sp)", "IB1\t#1(R)", "movB1\t(sp)+,R", NULL,
    "%n*,n", "F*", "mov\t#1(R),-(sp)", "I'\t$2,#1(R)", "mov\t(sp)+,R", NULL,
    NULL
};

/* cr34 - ! unary */
char *cr34_templates[] = {
    "%n,n", "FC", "beq\t1f", "clr\tR", "br\t2f", "1:\tmov\t$1,R", "2:", NULL,
    NULL
};

/* cr35 - & unary */
char *cr35_templates[] = {
    "%a,n", "mov\t$A1,R", NULL,
    NULL
};

/* cr29 - & unary of auto */
char *cr29_templates[] = {
    "%e,n", "mov\tr5,R", "add\tZ,R", NULL,
    NULL
};

/* cr36 - * unary */
char *cr36_templates[] = {
    "%abp*,n", "F", "movb\t(R),R", NULL,
    "%a*,n", "F", "mov\t(R),R", NULL,
    "%abp,n", "movb\t*A1,R", NULL,
    "%a,n", "mov\t*A1,R", NULL,
    "%nbp*,n", "F*", "movb\t*#1(R),R", NULL,
    "%n*,n", "F*", "mov\t*#1(R),R", NULL,
    "%nbp,n", "H*", "movb\t~(R),R", NULL,
    "%n,n", "H*", "mov\t~(R),R", NULL,
    NULL
};

/* cr37 - - unary */
char *cr37_templates[] = {
    "%n,n", "F", "neg\tR", NULL,
    NULL
};

/* cr38 - ~ */
char *cr38_templates[] = {
    "%n,n", "F", "com\tR", NULL,
    NULL
};

/* cr80 - = assignment */
char *cr80_templates[] = {
    "%a,n", "S", "movB1\tR,A1", NULL,
    "%n*,a", "F*", "movB1\tA2,#1(R)", "movB1\t#1(R),R", NULL,
    "%n*,e", "F*", "S1", "movB1\tR1,#1(R)", "mov\tR1,R", NULL,
    "%n*,n", "FS*", "S", "movB1\tR,*(sp)+", NULL,
    NULL
};

/* cr40 - + */
char *cr40_templates[] = {
    "%n,aw", "F", "I\tA2,R", NULL,
    "%n,ew*", "F", "S1*", "I\t#2(R1),R", NULL,
    "%n,e", "F", "S1", "I\tR1,R", NULL,
    "%n,nw*", "SS*", "F", "I\t*(sp)+,R", NULL,
    "%n,n", "SS", "F", "I\t(sp)+,R", NULL,
    NULL
};

/* cr42 - * multiply */
char *cr42_templates[] = {
    "%aw,a", "mov\tA1,(r4)+", "movB2\tA2,(r4)", "mov\t-(r4),R", NULL,
    "%n,a", "F", "mov\tR,(r4)+", "movB2\tA2,(r4)", "mov\t-(r4),R", NULL,
    "%n,e", "F", "S1", "mov\tR,(r4)+", "mov\tR1,(r4)", "mov\t-(r4),R", NULL,
    "%n,n", "FS", "S", "mov\t(sp)+,(r4)+", "mov\tR,(r4)", "mov\t-(r4),R", NULL,
    NULL
};

/* cr43 - / divide */
char *cr43_templates[] = {
    "%a,a", "movB1\tA1,(r4)", "movB2\tA2,div", "mov\tI,R", NULL,
    "%a,n", "S", "movB1\tA1,(r4)", "mov\tR,div", "mov\tI,R", NULL,
    "%n,a", "F", "mov\tR,(r4)", "movB2\tA2,div", "mov\tI,R", NULL,
    "%n,e", "F", "S1", "mov\tR,(r4)", "mov\tR1,div", "mov\tI,R", NULL,
    "%e,n", "S", "F1", "mov\tR1,(r4)", "mov\tR,div", "mov\tI,R", NULL,
    "%n,n", "FS", "S", "mov\t(sp)+,(r4)", "mov\tR,div", "mov\tI,R", NULL,
    NULL
};

/* cr60 - relationals */
char *cr60_templates[] = {
    "%n,n", "HC", "I\t2f", "clr\tR", "br\t1f", "2:\tmov\t$1,R", "1:", NULL,
    NULL
};

/* cr70 - =+ compound assignment */
char *cr70_templates[] = {
    "%aw,aw", "I\tA2,A1", "mov\tA1,R", NULL,
    "%aw,nw*", "S*", "I\t#2(R),A1", "mov\tA1,R", NULL,
    "%aw,n", "S", "I\tR,A1", "mov\tA1,R", NULL,
    "%a,n", "S", "movB1\tA1,R1", "I\tR1,R", "movB1\tR,A1", NULL,
    "%n*,n", "SS", "F*", "movB1\t#1(R),R1", "I\t(sp)+,R1", "movB1\tR1,#1(R)", "mov\tR1,R", NULL,
    NULL
};

/* Main operator table mapping opcodes to templates */
typedef struct {
    int opcode;
    char **templates;
    char *name;
} OpEntry;

OpEntry regtab_entries[] = {
    {20, cr20_templates, "name/const"},
    {21, cr20_templates, "const"},
    {22, cr20_templates, "string"},
    {30, cr30_templates, "++pre"},
    {31, cr30_templates, "--pre"},
    {32, cr32_templates, "++post"},
    {33, cr32_templates, "--post"},
    {34, cr34_templates, "!"},
    {35, cr35_templates, "&unary"},
    {29, cr29_templates, "&auto"},
    {36, cr36_templates, "*unary"},
    {37, cr37_templates, "-unary"},
    {38, cr38_templates, "~"},
    {80, cr80_templates, "="},
    {40, cr40_templates, "+"},
    {41, cr40_templates, "-"},  /* - like + */
    {42, cr42_templates, "*"},
    {43, cr43_templates, "/"},
    {60, cr60_templates, "=="},
    {61, cr60_templates, "!="},
    {62, cr60_templates, "<="},
    {63, cr60_templates, "<"},
    {64, cr60_templates, ">="},
    {65, cr60_templates, ">"},
    {70, cr70_templates, "=+"},
    {71, cr70_templates, "=-"},
    {0, NULL, NULL}  /* End marker */
};

/* Template matching and code generation functions */
int match_template(char *pattern, int *operands, int op_count) {
    /* Simple pattern matching - can be enhanced */
    if (strcmp(pattern, "%n,n") == 0) return 1;  /* Any operand, any operand */
    if (strcmp(pattern, "%a,n") == 0) return 1;  /* Address, any operand */
    if (strcmp(pattern, "%z,n") == 0) return operands[0] == 0;  /* Zero, any */
    return 0;  /* No match */
}

void generate_code(char **template_code, int *operands, int reg) {
    char **instr = template_code;
    
    while (*instr) {
        char *code = *instr;
        
        /* Process template macros */
        if (strcmp(code, "F") == 0) {
            cprintf("\t; F - fetch operand\n");
        } else if (strcmp(code, "S") == 0) {
            cprintf("\t; S - store operand\n");
        } else if (strncmp(code, "mov", 3) == 0) {
            /* Handle mov instructions with substitutions */
            if (strstr(code, "R")) {
                char output[256];
                strcpy(output, code);
                /* Replace R with actual register */
                char *r_pos = strstr(output, "R");
                if (r_pos) {
                    *r_pos = '0' + reg;  /* Simple register substitution */
                }
                cprintf("\t%s\n", output);
            } else {
                cprintf("\t%s\n", code);
            }
        } else if (strncmp(code, "clr", 3) == 0) {
            cprintf("\tclr\tr%d\n", reg);
        } else if (strncmp(code, "I", 1) == 0) {
            /* Handle I macro - could be add/sub depending on context */
            cprintf("\tadd\t$%d,r%d\n", operands[1], reg);
        } else {
            cprintf("\t%s\n", code);
        }
        instr++;
    }
}

/* Main code generation function */
int generate_regtab_code(int opcode, int *operands, int op_count, int reg) {
    OpEntry *entry = regtab_entries;
    
    /* Find the opcode in the table */
    while (entry->opcode != 0) {
        if (entry->opcode == opcode) {
            char **templates = entry->templates;
            
            /* Try each template until one matches */
            while (*templates) {
                char *pattern = *templates++;
                
                if (match_template(pattern, operands, op_count)) {
                    cprintf("; %s operation\n", entry->name);
                    
                    /* Collect template code until NULL */
                    char *code_start[20];
                    int code_count = 0;
                    while (*templates && code_count < 19) {
                        code_start[code_count++] = *templates++;
                    }
                    code_start[code_count] = NULL;
                    
                    generate_code(code_start, operands, reg);
                    return 1;  /* Success */
                }
                
                /* Skip to next template */
                while (*templates && *templates != NULL) templates++;
                if (*templates == NULL) templates++;
            }
            break;
        }
        entry++;
    }
    
    return 0;  /* No match found */
}

/* Initialize the real register table */
void init_real_regtab(void) {
    printf("; Real 1972 Bell Labs register allocation table initialized\n");
}

/* Test function */
void test_regtab(void) {
    int operands[2] = {5, 3};
    
    printf("; Testing assignment (=) with constants\n");
    generate_regtab_code(80, operands, 2, 0);
    
    printf("; Testing addition (+) with constants\n");
    generate_regtab_code(40, operands, 2, 0);
}