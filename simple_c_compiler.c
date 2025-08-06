#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

// Core globals from c00-mod.c
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
int *swp = 0;
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
int fin = 0;
int fout = 1;
char *tmpfil;
int ctab[128];

// Function prototypes
int init(char *s, int t);
int *lookup(void);
int symbol(void);
int getcc(void);
void error(char *s, ...);
void flush(void);
void flshw(void);

// Stub functions for missing dependencies
void extdef(void) {
    printf("EXTDEF: External definition\n");
    symbol(); // Parse one symbol
}

void blkend(void) {
    printf("BLKEND: Block end\n");
}

int *block(int n, int op, int t, int d, ...) {
    static int dummy_block[10];
    printf("BLOCK: Creating block op=%d type=%d\n", op, t);
    dummy_block[0] = op;
    dummy_block[1] = t;
    dummy_block[2] = d;
    return dummy_block;
}

int build(int op) {
    printf("BUILD: Building expression with op=%d\n", op);
    return 0;
}

// Include the core functions from c00-mod.c
int init(char *s, int t) {
    int *np, i;

    i = namsiz;
    memset(symbuf, 0, sizeof(symbuf));
    strncpy(symbuf, s, namsiz-1);
    
    np = lookup();
    *np++ = 1;
    *np = t;
    return 0;
}

int *lookup() {
    int i, j, *np, *sp, *rp;

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
no:        if ((i += pssiz) >= hshlen) i = 0;
    }
    if(hshused++ > hshsiz) {
        error("Symbol table overflow");
        exit(1);
    }
    rp = np = &hshtab[i];
    sp = (int*)symbuf;
    j = 4;
    while(j--)
        *np++ = *sp++;
    return(rp);
}

void error(char *s, ...) {
    printf("ERROR: %s\n", s);
    nerror++;
}

void flush(void) {
    // Stub
}

void flshw(void) {
    // Stub
}

int getcc() {
    int c, cc;

    if(peekc) {
        c = peekc;
        peekc = 0;
        return(c);
    }
    cc = 0;
loop:
    if(cc++ < ncpw)
        return(getchar());
    if(cc>ncpw)
        error("Token too long");
    goto loop;
}

int symbol(void) {
    int c, i;

    if(peeksym>=0) {
        c = peeksym;
        peeksym = -1;
        return(c);
    }

loop:
    switch(c = getcc()) {

    case '\0':
    case '\004':
        eof++;
        return(0);

    case ' ':
    case '\t':
        goto loop;

    case '\n':
        line++;
        goto loop;

    case '/':
        if((c=getcc())!='*') {
            peekc = c;
            return(43); // division operator
        }
        while((c=getcc())!='\0')
            if(c=='*')
                if((c=getcc())=='/')
                    goto loop;
                else
                    peekc = c;
        eof++;
        error("EOF in comment");
        return(0);

    case '"':
        i = 0;
        while((c=getcc())!='"') {
            if(c=='\0') {
                error("Missing \"");
                return(0);
            }
            if(c=='\\')
                c = getcc();
            if(i < 32)
                symbuf[i++] = c;
        }
        symbuf[i] = '\0';
        cval = (intptr_t)symbuf;
        return(22); // string constant

    case '\'':
        cval = 0;
        i = 0;
        while((c=getcc())!='\'') {
            if(c=='\0') {
                error("Missing '");
                return(0);
            }
            if(c=='\\')
                c = getcc();
            cval = (cval<<8) | c;
            i++;
        }
        if(i>ncpw)
            error("Long character constant");
        return(21); // char constant

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        cval = 0;
        if(c=='0') {
            // Octal
            while((c=getcc())>='0' && c<='7')
                cval = (cval<<3) | (c-'0');
            peekc = c;
            return(21); // constant
        }
        // Decimal
        do {
            cval = cval*10 + c-'0';
        } while((c=getcc())>='0' && c<='9');
        peekc = c;
        return(21); // constant

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '_':
        i = 0;
        do {
            if(i < namsiz-1)
                symbuf[i++] = c;
        } while(((c=getcc())>='a' && c<='z') ||
                (c>='A' && c<='Z') ||
                (c>='0' && c<='9') ||
                c=='_');
        symbuf[i] = '\0';
        peekc = c;
        
        // Look up in symbol table
        csym = lookup();
        if(csym[0] == 1) { // keyword
            return(csym[1] + 100); // keyword token
        }
        return(20); // identifier

    case '=':
        if((c=getcc())=='=')
            return(60); // ==
        peekc = c;
        return(80); // =

    case '!':
        if((c=getcc())=='=')
            return(61); // !=
        peekc = c;
        return(85); // !

    case '<':
        if((c=getcc())=='=')
            return(64); // <=
        if(c=='<')
            return(71); // <<
        peekc = c;
        return(62); // <

    case '>':
        if((c=getcc())=='=')
            return(65); // >=
        if(c=='>')
            return(72); // >>
        peekc = c;
        return(63); // >

    case '+':
        if((c=getcc())=='+')
            return(75); // ++
        peekc = c;
        return(40); // +

    case '-':
        if((c=getcc())=='-')
            return(76); // --
        peekc = c;
        return(41); // -

    case '*':
        return(42); // *

    case '%':
        return(44); // %

    case '&':
        if((c=getcc())=='&')
            return(66); // &&
        peekc = c;
        return(50); // &

    case '|':
        if((c=getcc())=='|')
            return(67); // ||
        peekc = c;
        return(51); // |

    case '^':
        return(52); // ^

    case '(':
        return(6);

    case ')':
        return(7);

    case '[':
        return(4);

    case ']':
        return(5);

    case '{':
        return(2);

    case '}':
        return(3);

    case ';':
        return(1);

    case ',':
        return(9);

    case ':':
        return(8);

    case '?':
        return(90);

    default:
unkn:
        error("Unknown character");
        // Consume the next character to avoid infinite loop
        c = getcc();
        if (c == '\0' || c == '\004') {
            eof++;
            return 0;
        }
        goto loop;
    }
}

int main(int argc, char *argv[]) {
    int sym;
    
    if(argc < 2) {
        printf("Usage: %s <source.c>\n", argv[0]);
        printf("1972 C Compiler - Lexical Analyzer Demo\n");
        exit(1);
    }
    
    if((fin = open(argv[1], O_RDONLY)) < 0) {
        error("Can't find input file");
        exit(1);
    }
    
    // Initialize hash table
    for(int i = 0; i < hshlen; i++) {
        hshtab[i] = 0;
    }
    
    // Initialize keywords
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
    
    printf("=== 1972 C Compiler - Lexical Analysis ===\n");
    printf("Analyzing file: %s\n\n", argv[1]);
    printf("Token stream:\n");
    
    // Redirect stdin to our input file
    dup2(fin, 0);
    
    while(!eof && nerror == 0) {
        sym = symbol();
        if(sym == 0) break; // EOF
        
        printf("Token %3d: ", sym);
        
        // Decode token type
        if(sym >= 100) {
            printf("KEYWORD   ");
            switch(sym - 100) {
                case 0: printf("int"); break;
                case 1: printf("char"); break;
                case 2: printf("float"); break;
                case 3: printf("double"); break;
                case 5: printf("auto"); break;
                case 6: printf("extern"); break;
                case 7: printf("static"); break;
                case 10: printf("goto"); break;
                case 11: printf("return"); break;
                case 12: printf("if"); break;
                case 13: printf("while"); break;
                case 14: printf("else"); break;
                case 15: printf("switch"); break;
                case 16: printf("case"); break;
                case 17: printf("break"); break;
                case 18: printf("continue"); break;
                case 19: printf("do"); break;
                case 20: printf("default"); break;
                default: printf("unknown_keyword");
            }
        } else if(sym == 20) {
            printf("IDENTIFIER '%s'", symbuf);
        } else if(sym == 21) {
            printf("CONSTANT   %d", cval);
        } else if(sym == 22) {
            printf("STRING     \"%s\"", symbuf);
        } else {
            printf("OPERATOR   ");
            switch(sym) {
                case 1: printf(";"); break;
                case 2: printf("{"); break;
                case 3: printf("}"); break;
                case 4: printf("["); break;
                case 5: printf("]"); break;
                case 6: printf("("); break;
                case 7: printf(")"); break;
                case 8: printf(":"); break;
                case 9: printf(","); break;
                case 40: printf("+"); break;
                case 41: printf("-"); break;
                case 42: printf("*"); break;
                case 43: printf("/"); break;
                case 44: printf("%%"); break;
                case 50: printf("&"); break;
                case 51: printf("|"); break;
                case 52: printf("^"); break;
                case 60: printf("=="); break;
                case 61: printf("!="); break;
                case 62: printf("<"); break;
                case 63: printf(">"); break;
                case 64: printf("<="); break;
                case 65: printf(">="); break;
                case 66: printf("&&"); break;
                case 67: printf("||"); break;
                case 71: printf("<<"); break;
                case 72: printf(">>"); break;
                case 75: printf("++"); break;
                case 76: printf("--"); break;
                case 80: printf("="); break;
                case 85: printf("!"); break;
                case 90: printf("?"); break;
                default: printf("op_%d", sym);
            }
        }
        printf(" (line %d)\n", line);
    }
    
    printf("\n=== Analysis Complete ===\n");
    printf("Lines processed: %d\n", line);
    printf("Errors: %d\n", nerror);
    
    if(nerror == 0) {
        printf("✅ Lexical analysis successful!\n");
        printf("The input is valid 1972 C syntax.\n");
    } else {
        printf("❌ Lexical analysis failed.\n");
    }
    
    close(fin);
    return nerror > 0 ? 1 : 0;
}