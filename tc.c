/* Chris Curl, MIT license. */
/* Based on work by Marc Feeley (2001), MIT license. */
/* Please see the README.md for details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#define SYMBOLS_SZ   1000
#define CODE_SZ     25000
#define NODES_SZ    10000

#define BTWI(n,l,h) ((l<=n)&&(n<=h))

typedef unsigned char byte;

typedef struct { char type, name[32]; int sz; long val; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

// Tokens - NOTE: the first 8 must match the words list in tc.c
enum {
    DO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK, VOID_TOK, INT_TOK, BYTE_TOK, RET_TOK
    , TOK_LBRA, TOK_RBRA, TOK_LPAR, TOK_RPAR, TOK_LARR, TOK_RARR
    , TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH
    , TOK_LT, TOK_EQ, TOK_GT, TOK_NEQ
    , TOK_SET, TOK_NUM, TOK_ID, TOK_FUNC, TOK_STR
    , TOK_OR, TOK_AND, TOK_XOR, TOK_LOR, TOK_LAND
    , TOK_SEMI, EOI
};

// Syntax tree node types
enum {
    ND_VAR, ND_CONST, ND_STR, ND_ADD, ND_SUB, ND_MUL, ND_DIV
    , ND_LT, ND_EQ, ND_GT, ND_NEQ, ND_SET
    , ND_AND, ND_OR, ND_XOR, ND_LAND, ND_LOR
    , ND_FUNC_CALL, ND_FUNC_DEF
    , ND_IF1, ND_IF2, ND_WHILE, ND_DO, ND_EMPTY, ND_SEQ
    , ND_RET
};

// VM opcodes
enum {
    NOP=0x90, IADD=0x01
    , IAND=0x21, IOR=0x09, IXOR=0x31
    , ISUB=0x29, MULDIV=0xf7
    , IRET=0xc3
    , MovRR=0x89, MovIMM=0xb8, MovFet=0xa1, MovSto=0xa3
    , XCHGAB=0x93, ICMP=0x39
    , JZ=0x74, JNZ=0x75, JGE=0x7d, JLE=0x7e
    , INCDX=0x42
};

byte vm[CODE_SZ];
extern int findSymbolVal(char type, long val);
extern void dumpSymbols(int details, FILE *toFP);

 /*---------------------------------------------------------------------------*/
 /* Lexer. */

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens
char *words[] = { "do", "else", "if"
    , "while", "void", "int", "char"
    , "return", NULL};

int ch = ' ', tok, int_val;
char id_name[256];
FILE *input_fp = NULL;
char cur_line[256] = {0};
int cur_off = 0, cur_lnum = 0, is_eof = 0;

void msg(char *s, int cr, int ln) {
    if (ln) {
        printf("\nat (%d,%d): %s", cur_lnum, cur_off, s);
        printf("\n%s", cur_line);
        for (int i=2; i<cur_off; i++) { fprintf(stdout, " "); }
        printf("^\n");
        return;
    }
    printf("%s%s", s, cr ? "\n" : " ");
}
void warn(char *s)  { msg(s,1,1); }
void error(char *err) { msg(err,1,1); exit(1); }
void syntax_error() { error("syntax error"); }

int isAlpha(int ch) { return BTWI(ch, 'A', 'Z') || BTWI(ch, 'a', 'z') || (ch == '_'); }
int isNum(int ch) { return BTWI(ch, '0', '9'); }
int isAlphaNum(int ch) { return isAlpha(ch) || isNum(ch); }

void next_line() {
    cur_off = 0;
    cur_lnum++;
    if (fgets(cur_line, 256, input_fp) != cur_line) {
        is_eof = 1;
    }
    //printf("----- %s\n", cur_line);
}

void next_ch() {
    if (is_eof) { ch = EOF; return; }
    if (cur_line[cur_off] == 0) {
        next_line();
        if (is_eof) { ch = EOF; return; }
    }
    ch = cur_line[cur_off++];
    if (ch == 9) { ch = cur_line[cur_off-1] = 32; }
}

void next_token() {
    again:
    while (BTWI(ch,1,32)) { next_ch(); }
    switch (ch) {
    case EOF: tok = EOI; break;
    case '{': next_ch(); tok = TOK_LBRA;  break;
    case '}': next_ch(); tok = TOK_RBRA;  break;
    case '(': next_ch(); tok = TOK_LPAR;  break;
    case ')': next_ch(); tok = TOK_RPAR;  break;
    case '[': next_ch(); tok = TOK_LARR;  break;
    case ']': next_ch(); tok = TOK_RARR;  break;
    case '+': next_ch(); tok = TOK_PLUS;  break;
    case '-': next_ch(); tok = TOK_MINUS; break;
    case '*': next_ch(); tok = TOK_STAR;  break;
    case '/': next_ch(); tok = TOK_SLASH;
        if (ch == '/') { // Line comment?
            while ((ch != 10) && (ch != EOF)) { next_ch(); }
            goto again;
        }
        break;
    case ';': next_ch(); tok = TOK_SEMI; break;
    case '<': next_ch(); tok = TOK_LT;   break;
    case '>': next_ch(); tok = TOK_GT;   break;
    case '^': next_ch(); tok = TOK_XOR;  break;
    case '=': next_ch(); tok = TOK_SET;
        if (ch == '=') { tok = TOK_EQ; next_ch(); }
        break;
    case '!': next_ch();
        if (ch == '=') { tok = TOK_NEQ; next_ch(); }
        else { syntax_error(); }
        break;
    case '|': next_ch(); tok = TOK_OR;
        if (ch == '|') { tok = TOK_LOR; next_ch(); }
        break;
    case '&': next_ch(); tok = TOK_AND;
        if (ch == '&') { tok = TOK_LAND; next_ch(); }
        break;
    case '"': next_ch(); tok = TOK_STR;
        int len = 0;
        while (ch != '"') {
            id_name[len++] = ch;
            next_ch();
            if (ch == EOF) { syntax_error(); }
        }
        id_name[len] = 0;
        next_ch();
        break;
    default:
        if (isNum(ch)) {
            int_val = 0; /* missing overflow check */
            while (isNum(ch)) { int_val = int_val  *10 + (ch - '0'); next_ch(); }
            tok = TOK_NUM;
        }
        else if (isAlpha(ch)) {
            int i = 0; /* missing overflow check */
            while (isAlphaNum(ch)) { id_name[i++] = ch; next_ch(); }
            id_name[i] = '\0';
            tok = 0;
            while ((words[tok] != NULL) && (strcmp(words[tok], id_name) != 0)) { tok++; }
            if (words[tok] == NULL) {
                tok = TOK_ID;
                if (ch == '(') {
                    next_ch();
                    if (ch == ')') { tok = TOK_FUNC; next_ch(); }
                    else { syntax_error(); }
                }
            }
        }
        else { syntax_error(); }
        break;
    }
}

void expect_token(int exp) {
    if (tok != exp) {
        printf("-expected token [%d], not[%d]-", exp, tok);
        syntax_error();
    }
    next_token();
}

/*---------------------------------------------------------------------------*/
/* Symbols */

SYM_T symbols[SYMBOLS_SZ];
int numSymbols = 0;

int findSymbol(char *name, char type) {
    int i = 0;
    while (i < numSymbols) {
        SYM_T *x = &symbols[i];
        if (strcmp(x->name, name) == 0) {
            if (x->type == type) { return i; }
            error("name already defined with different type.");
        }
        i=i+1;
    }
    return -1;
}

int genSymbol(char *name, char type) {
    int i = findSymbol(name, type);
    if (0 <= i) { return i; }
    i = numSymbols++;
    SYM_T *x = &symbols[i];
    // x->name = hAlloc(strlen(name) + 1);
    x->val = 0;
    x->type = type;
    x->sz = 4;
    strcpy(x->name, name);
    // printf("-new symbol:%s,%d-\n", x->name, x->type);
    return i;
}

char *genStringSymbolName() {
    static char name[32];
    static int strNum = 0;
    sprintf(name, "_s%03d_", ++strNum);
    return name;
}

void dumpSymbols(int details, FILE* toFP) {
    FILE *fp = toFP ? toFP : stdout;
    fprintf(fp, "\n; symbols: %d entries, %d used\n", SYMBOLS_SZ, numSymbols);
    fprintf(fp, "; num type size val       name\n");
    fprintf(fp, "; --- ---- ---- --------- -----------------\n");
    if (details) {
        for (int i = 0; i < numSymbols; i++) {
            SYM_T *x = &symbols[i];
            fprintf(fp, "; %-3d %-4d %-4d $%-8lX %s\n", 
                i, x->type, x->sz, x->val, x->name);
        }
    }
}

/*---------------------------------------------------------------------------*/
/* Parser. */

struct node_s { int kind; struct node_s *o1,  *o2,  *o3; int val, sval; };
typedef struct node_s node;
int num_nodes = 0;
node nodes[NODES_SZ];

node *new_node(int k) {
    if (NODES_SZ <= num_nodes) { error("no nodes!"); }
    node *x = &nodes[num_nodes++];
    x->kind = k;
    return x;
}

node *gen(int k, node *o1, node *o2) {
    node *x = new_node(k);
    x->o1 = o1;
    x->o2 = o2;
    return x;
}

node *paren_expr(); /* forward declaration */

/* <term> ::= <id> | <int> | <paren_expr> */
node *term() {
    node *x;
    if (tok == TOK_ID) {
        x = new_node(ND_VAR);
        x->sval = genSymbol(id_name, ND_VAR);
        x->val = x->sval;
        next_token();
    }
    else if (tok == TOK_NUM) {
        x = new_node(ND_CONST);
        x->val = int_val;
        next_token();
    }
    else if (tok == TOK_STR) {
        char *tmp = hAlloc(strlen(id_name)+1);
        strcpy(tmp, id_name);
        x = new_node(ND_STR);
        x->sval = genSymbol(genStringSymbolName(), ND_STR);
        symbols[x->sval].val = (long)tmp;
        next_token();
    }
    else x = paren_expr();
    return x;
}

/* <math_op> ::= "+" | "-" | "*" | "/" */
int mathop() {
    if (tok == TOK_PLUS) { return ND_ADD; }
    else if (tok == TOK_MINUS) { return ND_SUB; }
    else if (tok == TOK_STAR)  { return ND_MUL; }
    else if (tok == TOK_SLASH) { return ND_DIV; }
    else if (tok == TOK_LAND)  { return ND_LAND; }
    else if (tok == TOK_LOR)   { return ND_LOR; }
    else if (tok == TOK_AND)   { return ND_AND; }
    else if (tok == TOK_OR)    { return ND_OR; }
    else if (tok == TOK_XOR)   { return ND_XOR; }
    return 0;
}

/* <math> ::= <term> | <math> <math_op> <term> */
node *math() {
    node *x = term();
    while (mathop()) {
        x = gen(mathop(), x, 0);
        next_token();
        x->o2 = term();
    }
    return x;
}

/* <expr> ::= <math> | <math> <test-op> <math> */
node *expr() {
    node *x = math();
    if (tok == TOK_LT)  { next_token(); return gen(ND_LT,  x, math()); }
    if (tok == TOK_GT)  { next_token(); return gen(ND_GT,  x, math()); }
    if (tok == TOK_EQ)  { next_token(); return gen(ND_EQ,  x, math()); }
    if (tok == TOK_NEQ) { next_token(); return gen(ND_NEQ, x, math()); }
    return x;
}

/* <paren_expr> ::= "(" <expr> ")" */
node *paren_expr() {
    node *x;
    expect_token(TOK_LPAR);
    x = expr();
    expect_token(TOK_RPAR);
    return x;
}

node *statement() {
    node *x = NULL;
    if (tok == IF_TOK) { /* "if" <paren_expr> <statement> */
        x = new_node(ND_IF1);
        next_token();
        x->o1 = paren_expr();
        x->o2 = statement();
        if (tok == ELSE_TOK) { /* ... "else" <statement> */
            x->kind = ND_IF2;
            next_token();
            x->o3 = statement();
        }
    }
    else if (tok == WHILE_TOK) { /* "while" <paren_expr> <statement> */
        x = new_node(ND_WHILE);
        next_token();
        x->o1 = paren_expr();
        x->o2 = statement();
    }
    else if (tok == TOK_FUNC) { /* <id> "();" */
        x = new_node(ND_FUNC_CALL);
        x->sval = genSymbol(id_name, TOK_FUNC);
        x->val = symbols[x->sval].val;
        next_token();
        expect_token(TOK_SEMI);
    }
    else if (tok == TOK_ID) { /* <id> "=" <expr> ";" */
        x = new_node(ND_VAR);
        x->sval = genSymbol(id_name, ND_VAR);
        x->val = x->sval;
        next_token();
        expect_token(TOK_SET);
        x = gen(ND_SET, x, expr());
        expect_token(TOK_SEMI);
    }
    else if (tok == DO_TOK) { /* "do" <statement> "while" <paren_expr> ";" */
        x = new_node(ND_DO);
        next_token();
        x->o1 = statement();
        expect_token(WHILE_TOK);
        x->o2 = paren_expr();
        expect_token(TOK_SEMI);
    }
    else if (tok == RET_TOK) { /* "return" ";"*/
        x = new_node(ND_RET);
        next_token();
        expect_token(TOK_SEMI);
    }
    else if (tok == TOK_SEMI) { /* ";" */
        x = new_node(ND_EMPTY);
        next_token();
    }
    else if (tok == TOK_LBRA) { /* "{" <statements> "}" */
        int seqNo = 1;
        x = new_node(ND_EMPTY);
        next_token();
        while (tok != TOK_RBRA) {
            x = gen(ND_SEQ, x, 0);
            x->val = seqNo;
            x->o2 = statement();
        }
        next_token();
    }
    else { syntax_error(); }
    return x;
}


/*---------------------------------------------------------------------------*/
/* Code generator. */
// change this to generate code for a different architecture
char *gLines[10000], * gCurLine;
int gHere, gLineSz;

// ----------------------------------------------------------

void gN(char *txt) {
    gLineSz = (int)strlen(txt)+1;
    gCurLine = hAlloc(gLineSz);
    gLines[gHere] = gCurLine;
    strcpy(gCurLine, txt);
    //printf("%03d: %s\n", gHere, gCurLine);
    gHere = gHere+1;
}

void gAppend(char *txt, int sp) {
    if (gLineSz < (int)(strlen(gCurLine) + strlen(txt) + 2)) {
        gLineSz = gLineSz + 16;
        gCurLine = hRealloc(gCurLine, gLineSz);
        gLines[gHere-1] = gCurLine;
    }
    if (sp) { strcat(gCurLine, " "); }
    strcat(gCurLine, txt);
    //printf(" --> %s\n", gCurLine);
}

void gInit() {
    gHere = 0;
}

int hole() { return gHere; }
void fix(int tgtLn, int tgt, char *pfx) {
    char x[32];
    sprintf(x, " .%s%d", pfx, tgt);
    gLines[tgtLn] = hRealloc(gLines[tgtLn], 64);
    strcat(gLines[tgtLn], x);
    // if (v == gHere) { sprintf(x, ".L%d:", v); gN(x); }
}

void gDump() { for (int i=0; i<gHere; i++) { printf("%s\n", gLines[i]); } }

void gBtoA() { gN("\tMOV EAX, EBX"); }
void gDtoA() { gN("\tMOV EAX, EDX"); }
void gAtoB() { gN("\tMOV EBX, EAX"); }

void gNOP() { gN("\tNOP"); }
void gPushA() { gN("\tPUSH EAX"); }
void gPopA()  { gN("\tPOP EAX"); }
void gPopB()  { gN("\tPOP EBX"); }
void gXorDD() { gN("\tXOR EDX, EDX"); }

void gMovIMM(int val) {
    char x[64];
    sprintf(x, "\tMOV EAX, %d", val);
    gN(x);
}

void gStore(int sym) {
    SYM_T *s = &symbols[sym];
    int off = 0, sz = s->sz;
    char x[64];
    char *reg = (sz==1) ? "AL" : "EAX";
    if (off) {
        sprintf(x, "\tMOV [%s+%d], %s", s->name, off, reg);
    } else {
        sprintf(x, "\tMOV [%s], %s", s->name, reg);
    }
    gN(x);
}

void gFetch(int sym) {
    SYM_T *s = &symbols[sym];
    int off = 0, sz = s->sz;
    char x[64];
    char *reg = (sz==1) ? "AL" : "EAX";
    gN("\tPUSH EAX");
    if (sz == 1) { gN("XOR EAX, EAX"); }
    if (off) {
        sprintf(x, "\tMOV %s, [%s+%d]", reg, s->name, off);
    } else {
        sprintf(x, "\tMOV %s, [%s]", reg, s->name);
    }
    gN(x);
}

void gLit(int v) { gPushA(); gMovIMM(v); }

void gPreCall() { gN("\tpush ebp"); gN("\tmov ebp, esp"); }
void gCall(int fn) {
    gN("\tCALL");
    gAppend(symbols[fn].name, 1);
}
void gReturn() { gN("\tRET"); gN(""); }
void gPostCall() { gN("\tmov esp, ebp"); gN("\tpop ebp"); }
void gAdd() { gPopB(); gN("\tADD EAX, EBX"); }
void gSub() { gAtoB(); gPopA(); gN("\tSUB EAX, EBX"); }
void gMul() { gPopB(); gN("\tIMUL EBX"); }
void gDiv() { gAtoB(); gPopA(); gN("\tIDIV EAX"); }
void gTestA() { gN("\tTEST EAX, EAX"); gN("\tPOP EAX"); }
void gCC(char *op)  {
    gN("\tXOR EDX, EDX");
    gPopB(); gN("\tCMP EAX, EBX");
    gN("\t"); gAppend(op, 0); gAppend("@f", 1);
    gN("\tINC DX");
    gN("@@:\tMOV EAX, EDX");
}
void gLT()  { gCC("JGE"); }
void gGT()  { gCC("JLE"); }
void gEQ()  { gCC("JNE"); }
void gNEQ() { gCC("JE"); }
void gAnd() { gPopB(); gN("\tAND EAX, EBX"); }
void gOr()  { gPopB(); gN("\tOR EAX, EBX"); }
void gXor() { gPopB(); gN("\tXOR EAX, EBX"); }
int gNewTag(int ln, char *pfx) {
    char x[64];
    sprintf(x, ".%s%d:", pfx, ln);
    gN(x);
    return ln;
}
void gJmpTo(int ln, char *op, char *pfx) {
    char x[64];
    sprintf(x, "\t%s .%s%d", op, pfx, ln);
    gN(x);
}

void gWinLin(int seg) {
#ifdef _WIN32
    // Windows (32-bit)
    if (seg == 'C') {
        gN("format PE console");
        gN("include 'win32ax.inc'");
        gN(";================== code =====================");
        gN(".code\nentry main");
        gN(";================== library ==================");
        int sym = genSymbol("exit", TOK_FUNC);
        gN("exit:\tret");
        gN(";=============================================");
    } else if (seg == 'D') {
        gN(";================== data =====================");
        gN(".data");
        gN(";=============================================");
    }
#else
    // Linux (32-bit)
    if (seg == 'C') {
        gN("format ELF executable");
        gN(";================== code =====================");
        gN("segment readable executable\nentry main");
        gN(";================== library ==================");
        int sym = genSymbol("exit", TOK_FUNC);
        sym = genSymbol("_pc_buf", 0);
        gN("exit:\n\tMOV EAX, 1\n\tXOR EBX, EBX\n\tINT 0x80");
        gN("putc:\n\tMOV [_pc_buf], EAX");
        gN("MOV EAX, 4");
        gN("MOV EBX, 0");
        gN("LEA ECX, [_pc_buf]");
        gN("MOV EDX, 1");
        gN("INT 0x80\n");
        gN(";=============================================");
    } else if (seg == 'D') {
        gN(";================== data =====================");
        gN("segment readable writeable");
        gN(";=============================================");
    }
#endif
}
// ----------------------------------------------------------

void c(node *x) {
    int p1, p2, p3;
    switch (x->kind) {
        case ND_VAR:   gFetch(x->sval); break;
        case ND_CONST: gLit(x->val); break;
        case ND_STR:   gN("\tPUSH EAX\n\tLEA EAX, [");
            gAppend(symbols[x->sval].name, 0);
            gAppend("]", 0);
            break;
        case ND_ADD:   c(x->o1); c(x->o2); gAdd();  break;
        case ND_MUL:   c(x->o1); c(x->o2); gMul();  break;
        case ND_SUB:   c(x->o1); c(x->o2); gSub();  break;
        case ND_DIV:   c(x->o1); c(x->o2); gDiv();  break;
        case ND_LT:    c(x->o1); c(x->o2); gLT();   break;
        case ND_GT:    c(x->o1); c(x->o2); gGT();   break;
        case ND_EQ:    c(x->o1); c(x->o2); gEQ();   break;
        case ND_NEQ:   c(x->o1); c(x->o2); gNEQ();  break;
        case ND_LAND:  c(x->o1); c(x->o2); gAnd();  break;
        case ND_LOR:   c(x->o1); c(x->o2); gOr();   break;
        case ND_AND:   c(x->o1); c(x->o2); gAnd();  break;
        case ND_OR:    c(x->o1); c(x->o2); gOr();   break;
        case ND_XOR:   c(x->o1); c(x->o2); gXor();  break;
        case ND_SET:   c(x->o2); gStore(x->o1->sval); gN("\tPOP EAX");
            break;
        case ND_IF1:  gN("\t; IF ..."); c(x->o1);
            gTestA(); p1 = gHere; gN("\tJZ");
            gN("\t; THEN ..."); c(x->o2);
            fix(p1, gHere, "END");
            gNewTag(gHere, "END");
            break;
        case ND_IF2:  gN("\t; IF ... ELSE ..."); c(x->o1);
            gTestA(); p1 = gHere; gN("\tJZ");
            gN("\t; THEN ..."); c(x->o2);
            p2 = gHere; gN("\tJMP");
            fix(p1, gHere, "ELSE");
            gNewTag(gHere, "ELSE");
            gN("\t; ELSE ...");
            c(x->o3);
            fix(p2, gHere, "END");
            gNewTag(gHere, "END");
            break;
        case ND_WHILE: p1 = gNewTag(gHere, "WS");
            c(x->o1);
            gTestA();
            p2 = gHere;
            gN("\tJZ");
            c(x->o2);
            gJmpTo(p1, "JMP", "WS");
            p3 = gNewTag(gHere, "WE");
            gAppend("", 1);
            fix(p2, p3, "WE");
            break;
        case ND_DO: p1 = gHere; gNewTag(p1, "DS");
            c(x->o1);
            c(x->o2);
            gTestA();
            gJmpTo(p1, "JNZ", "DS"); break;
        case ND_EMPTY: break;
        case ND_SEQ: c(x->o1); c(x->o2); break;
        case ND_FUNC_CALL: 
            gPreCall();
            gCall(x->sval);
            gPostCall(); break;
        case ND_FUNC_DEF: c(x->o1); break;
        case ND_RET: gReturn(); break;
    }
}

/*---------------------------------------------------------------------------*/
/* Definitions. */

void defSize(int type, int sym) {
    // check for ";" or "[" <int> "];"
    symbols[sym].sz = (type == INT_TOK) ? 4 : 1;
    if (tok == TOK_SEMI) { next_token(); return; }
    expect_token(TOK_LARR);
    symbols[sym].sz = int_val;
    if (type == INT_TOK) { symbols[sym].sz *= 4; }
    expect_token(TOK_NUM);
    expect_token(TOK_RARR);
    expect_token(TOK_SEMI);
}

node *defs(node *st) {
    node *x = st;
    next_token();
    while (1) {
        if (tok == EOI) { break; }
        if (tok == VOID_TOK) {
            int seqNo = 1;
            next_token(); expect_token(TOK_FUNC);
            int sym = genSymbol(id_name, TOK_FUNC);
            symbols[sym].val = sym;
            gN(id_name);
            gAppend(":", 0);
            x = gen(ND_FUNC_DEF, NULL, NULL);
            x->sval = sym;
            if (tok != TOK_LBRA) error("'{' expected.");
            x->o1 = statement();
            c(x);
            gReturn();
            continue;
        }
        if (tok == INT_TOK) {
            next_token(); expect_token(TOK_ID);
            int sym = genSymbol(id_name, ND_VAR);
            defSize(INT_TOK, sym);
            continue;
        }
        if (tok == BYTE_TOK) {
            next_token(); expect_token(TOK_ID);
            int sym = genSymbol(id_name, ND_VAR);
            defSize(BYTE_TOK, sym);
            continue;
        }
        syntax_error();
    }
    return st;
}

/*---------------------------------------------------------------------------*/
/* Main program. */

int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : NULL;
    if (fn) { input_fp = fopen(fn, "rt"); }
    if (!input_fp) { input_fp = stdin; }
    hInit(0);
    gInit();
    gN("; TC source file: ");
    gAppend(fn ? fn : "stdin", 0);
    gWinLin('C');
    defs(NULL);
    if (input_fp != stdin) { fclose(input_fp); }
    // printf("; %d lines, %d nodes\n", gHere, num_nodes);

    gWinLin('D');
    for (int i=0; i<gHere; i++) { fprintf(stdout, "%s\n", gLines[i]); }

    for (int i=0; i<numSymbols; i++) {
        SYM_T *s = &symbols[i];
        if (s->type == ND_VAR) { printf("%s:\tdd 0\n", s->name); }
        if (s->type == ND_STR) { printf("%s:\tdb \"%s\", 0\n",s->name, (char*)s->val); }
    }

    dumpSymbols(1, 0);

    return 0;
}
