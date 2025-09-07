/* Chris Curl, MIT license. */
/* Please see the README.md for details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#define SYMBOLS_SZ   1000
#define CODE_SZ     25000
#define NODES_SZ    10000

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
#define G printf
#define P(str) printf("%s",str)

typedef unsigned char byte;

typedef struct { char type, name[23], asm_name[8]; char *strVal; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

// Tokens - NOTE: the first 8 must match the words list in tc.c
enum {
    NO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK, VOID_TOK, INT_TOK, CHAR_TOK, RET_TOK
    , TOK_THEN, TOK_END, TOK_BEGIN, TOK_UNTIL, TOK_AGAIN, TOK_DEF
    , TOK_LBRA, TOK_RBRA, TOK_LPAR, TOK_RPAR, TOK_LARR, TOK_RARR, TOK_COMMA
    , TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_INC, TOK_DEC, TOK_PLEQ
    , TOK_LT, TOK_EQ, TOK_GT, TOK_NEQ
    , TOK_SET, TOK_NUM, TOK_ID, TOK_FUNC, TOK_STR, TOK_REG
    , TOK_OR, TOK_AND, TOK_XOR, TOK_LOR, TOK_LAND
    , TOK_SEMI, EOI
};

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens
char *words[] = { "", "else", "if" , "while", "void", "int", "char" , "return",
    "then", "end", "begin", "until", "again", "def", NULL
};

int ch = ' ', tok, int_val;
char id_name[256];
FILE *input_fp = NULL;
char cur_line[256] = {0};
int cur_off = 0, cur_lnum = 0, is_eof = 0;
int tgtReg = 0, tok_len;
char regs[6][4] = { "EAX", "EBX", "ECX", "EDX", "ESI", "EDI" };
char *lReg, *rReg;
int code[CODE_SZ], codeSz = 0;
int arg1[CODE_SZ];

void statement();
void statements();
void expr();
int term();

void msg(int fatal, char *s) {
    printf("\n%s at(%d, %d)", s, cur_lnum, cur_off);
    printf("\n%s", cur_line);
    for (int i = 2; i < cur_off; i++) { printf(" "); } printf("^");
    if (fatal) { fprintf(stderr, "\n%s (see output for details)\n", s); exit(1); }
}
void syntax_error() { msg(1, "syntax error"); }

int isAlpha(int ch) { return BTWI(ch, 'A', 'Z') || BTWI(ch, 'a', 'z') || (ch == '_'); }
int isNum(int ch) { return BTWI(ch, '0', '9'); }
int isAlphaNum(int ch) { return isAlpha(ch) || isNum(ch); }

void next_line() {
    cur_off = 0;
    cur_lnum++;
    if (fgets(cur_line, 256, input_fp) != cur_line) {
        is_eof = 1;
    }
    // int l = strlen(cur_line);
    // if ((0 < l) && (cur_line[l-1] == 10)) { cur_line[l-1] = 0; }
    // else { l++; }
    // printf("\n\t; %s", cur_line);
    // cur_line[l-1] = 10;
    // cur_line[l] = 0;
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

/*---------------------------------------------------------------------------*/
/* Lexer */
void next_token() {
    again:
    while (BTWI(ch,1,32)) { next_ch(); }
    switch (ch) {
    case EOF: tok = EOI; break;
    //case '{': next_ch(); tok = TOK_LBRA;  break;
    //case '}': next_ch(); tok = TOK_RBRA;  break;
    //case '(': next_ch(); tok = TOK_LPAR;  break;
    //case ')': next_ch(); tok = TOK_RPAR;  break;
    case '[': next_ch(); tok = TOK_LARR;  break;
    case ']': next_ch(); tok = TOK_RARR;  break;
    case '+': next_ch(); tok = TOK_PLUS;
        if (ch == '+') { next_ch(); tok = TOK_INC; }
        if (ch == '=') { next_ch(); tok = TOK_PLEQ; }
        break;
    case '-': next_ch(); tok = TOK_MINUS;
        if (ch == '-') { next_ch(); tok = TOK_DEC; }
        break;
    case '*': next_ch(); tok = TOK_STAR;  break;
    case '/': next_ch(); tok = TOK_SLASH;
        if (ch == '/') { // Line comment?
            while ((ch) && (ch != 10) && (ch != EOF)) { next_ch(); }
            goto again;
        }
        break;
    case ';': next_ch(); tok = TOK_SEMI;  break;
    case ',': next_ch(); tok = TOK_COMMA; break;
    case '<': next_ch(); tok = TOK_LT;    break;
    case '>': next_ch(); tok = TOK_GT;    break;
    case '^': next_ch(); tok = TOK_XOR;   break;
    case '=': next_ch(); tok = TOK_SET;
        if (ch == '=') { tok = TOK_EQ; next_ch(); }
        break;
    // case '!': next_ch();
    //     if (ch == '=') { tok = TOK_NEQ; next_ch(); }
    //     else { syntax_error(); }
    //     break;
    case '|': next_ch(); tok = TOK_OR;
    //    if (ch == '|') { tok = TOK_LOR; next_ch(); }
        break;
    case '&': next_ch(); tok = TOK_AND;
    //    if (ch == '&') { tok = TOK_LAND; next_ch(); }
        break;
    case '"': tok = TOK_STR;
        tok_len = 0;
        next_ch();
        while (ch != '"') {
            if (ch == EOF) { syntax_error(); }
            id_name[tok_len++] = ch;
            next_ch();
        }
        id_name[tok_len] = 0;
        next_ch();
        break;
    case '\'': next_ch(); int_val = ch; next_ch();
        if (ch == '\'') { next_ch(); tok = TOK_NUM; } else { syntax_error(); }
        break;
    default:
        if (isNum(ch)) {
            int_val = 0; /* missing overflow check */
            while (isNum(ch)) { int_val = int_val  *10 + (ch - '0'); next_ch(); }
            tok = TOK_NUM;
        }
        else if (isAlpha(ch)) {
            tok_len = 0; /* missing overflow check */
            while (isAlphaNum(ch)) { id_name[tok_len++] = ch; next_ch(); }
            id_name[tok_len] = '\0';
            // printf("\n; id_name='%s'", id_name);
            tok = 0;
            while ((words[tok] != NULL) && (strcmp(words[tok], id_name) != 0)) { tok++; }
            if (words[tok] == NULL) {
                tok = TOK_ID;
                if ((id_name[1] == 0) && (BTWI(id_name[0], 'A', 'Z'))) {
                    tok = TOK_REG;
                    int_val = id_name[0] - 'A';
                }
            }
        }
        else { syntax_error(); }
        break;
    }
}

void tokenShouldBe(int exp) {
    if (tok != exp) {
        printf("\n-expected token [%d], not[%d]-", exp, tok);
        syntax_error();
    }
}

void expectToken(int exp) { tokenShouldBe(exp); next_token(); }
void expectNext(int exp) { next_token(); expectToken(exp); }
void nextShouldBe(int exp) { next_token(); tokenShouldBe(exp); }
void tokenShouldNotBe(int x) { if (tok == x) { syntax_error(); } }

/*---------------------------------------------------------------------------*/
/* Symbols - 'C' = Char, 'I' = INT,  'F' = Function, 'S' = String, 'T' = Target */
SYM_T symbols[SYMBOLS_SZ];
int numSymbols = 0, localOff, paramOff;

char *symName(int sym) { return symbols[sym].name; }
char *asmName(int sym) { return symbols[sym].asm_name; }

int findSymbol(char *name, char type) {
    int i = 0;
    while (i < numSymbols) {
        SYM_T *x = &symbols[i];
        if (strcmp(x->name, name) == 0) {
            if (x->type == type) { return i; }
            return -1; // msg(0, "name already defined with different type.", 1, 1);
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
    x->strVal = NULL;
    x->type = type;
    strcpy(x->name, name);
    sprintf(x->asm_name, "%c%d", type, i);
    return i;
}

void dumpSymbols() {
    printf("\n; symbols: %d entries, %d used", SYMBOLS_SZ, numSymbols);
    printf("\n; ------------------------------------");
    for (int i = 0; i < numSymbols; i++) {
        SYM_T *x = &symbols[i];
        if (x->type == 'S') {      printf("\n%-10s\tdb \"%s\",0 ; %s", x->asm_name, x->strVal, x->name); }
        else if (x->type == 'I') { printf("\n%-10s\tdd 0 ; %s", x->asm_name, x->name); }
        else if (x->type == 'C') { printf("\n%-10s\tdb 256 dup(0) ; %s", x->asm_name, x->name); }
        else if (x->type == 'F') { printf("\n; %s ; Function %s", x->asm_name, x->name); }
        else if (x->type == 'T') { printf("\n; %s ; Target %s", x->asm_name, x->name); }
    }
    for (int i = 0; i < 26; i++) {
        printf("\n%-10c\tdd 0", 'A' + i);
        printf("\n%c_Sp      \tdd 0", 'A' + i);
        printf("\n%cStk      \tdd 32 dup(0)", 'A' + i);
    }
}

// ---------------------------------------------------------------------------
// IRL
enum { IRL_REG, IRL_NUM, IRL_STR, IRL_LIT
    , IRL_OP, IRL_LABEL, IRL_ID
    , IRL_FUNC, IRL_CALL, IRL_RET, IRL_JMP, IRL_JMPZ
    , IRL_ADD, IRL_SUB, IRL_IMUL, IRL_IDIV
    , IRL_LT, IRL_GT, IRL_EQ, IRL_NEQ
    , IRL_AND, IRL_OR, IRL_XOR
    , IRL_SHL, IRL_SHR // << and >>
    , IRL_INC, IRL_DEC // ++ and --
    , IRL_PLEQ         // +=
    , IRL_IDSET        // id = expr
    , IRL_REGSET       // reg = expr
    , IRL_IF, IRL_ELSE // for if/else
    , IRL_BEGIN, IRL_WHILE, IRL_UNTIL, IRL_AGAIN // loop constructs
};

int g0(int a) { code[++codeSz] = a; return codeSz; }
int g1(int a, int b) { g0(a); arg1[codeSz] = b; return codeSz; }

/*---------------------------------------------------------------------------*/
/* Parser / code generator. */  
void winLin(int seg) {
#ifdef _WIN32
    // Windows (32-bit)
    if (seg == 'C') {
        int s = genSymbol("exit", 'F');
        s = genSymbol("puts", 'F');
        s = genSymbol("putc", 'F');
        s = genSymbol("putd", 'F');
        s = genSymbol("pv", 'I');
        P("format PE console");
        P("\ninclude 'win32ax.inc'\n");
        P("\n; ======================================= ");
        P("\nsection '.code' code readable executable");
        P("\n;=======================================*/");
        P("\n\nstart: JMP main");
        P("\n;================== library ==================");
        P("\nexit:\n\tPUSH 0\n\tCALL [ExitProcess]\n");
        P("\nputs:\n\tcinvoke printf, \"%s\", [pv]");
        P("\n\tRET\n");
        P("\nputc:\n\tcinvoke printf, \"%c\", [pv]");
        P("\n\tRET\n");
        P("\nputd:\n\tcinvoke printf, \"%d\", [pv]");
        P("\n\tRET\n");
        P("\n;=============================================");
    }
    else if (seg == 'D') {
        P("\n\n;================== data =====================");
        P("\nsection '.data' data readable writeable");
        P("\n;=============================================");
    }
    else if (seg == 'I') {
        P("\n;====================================");
        P("\nsection '.idata' import data readable");
        P("\n; ====================================");
        P("\nlibrary msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'");
        P("\nimport msvcrt, printf,'printf', getch,'_getch'");
        P("\nimport kernel32, ExitProcess,'ExitProcess'\n");
    }
#else
    // Linux (32-bit)
    if (seg == 'C') {
        int s = genSymbol("exit", 'F');
        s = genSymbol("pv", 'I');
        s = genSymbol("_pc_buf", 'I');
        P("format ELF executable");
        P("\n;================== code =====================");
        P("\nsegment readable executable");
        P("\nentry main");
        P("\n;================== library ==================");
        P("\nexit:\n\tMOV EAX, 1\n\tXOR EBX, EBX\n\tINT 0x80\n");
        P("\nputs:\n\tMOV [_pc_buf], EAX\n\tRET");
        P("\nputd:\n\tMOV [_pc_buf], EAX\n\tRET");
        P("\nputc:\n\tMOV [_pc_buf], EAX");
        P("\n\tMOV EAX, 4");
        P("\n\tMOV EBX, 0");
        P("\n\tLEA ECX, [_pc_buf]");
        P("\n\tMOV EDX, 1");
        P("\n\tINT 0x80\n\tRET\n");
        P("\n;=============================================");
    }
    else if (seg == 'D') {
        P("\n;================== data =====================");
        P("\nsegment readable writeable");
        P("\n;=============================================");
    }
    else if (seg == 'I') {
        // P("\n;================== data =====================");
        // P("\nsegment readable writeable");
        // P("\n;=============================================");
    }
#endif
}

char *regName(int regNum) { return regs[regNum]; }
void parens() { expr(); tokenShouldBe(TOK_RPAR); }

int term() {
    if (tok == TOK_REG)  { g1(IRL_REG, int_val); return 1; }
    if (tok == TOK_ID)   { g1(IRL_ID,  genSymbol(id_name, 'I')); return 1; }
    if (tok == TOK_NUM)  { g1(IRL_LIT, int_val); return 1; }
    if (tok == TOK_STR)  { g1(IRL_STR, genSymbol(id_name, 'S')); return 1; }
    return 0;
}

void next_term() {
    next_token();
    if (term()) { return; }
    syntax_error();
}

int evalOp(int id) {
    if (id == TOK_PLUS) { return id; }
    else if (id == TOK_MINUS) { return id; }
    else if (id == TOK_STAR)  { return id; }
    else if (id == TOK_SLASH) { return id; }
    else if (id == TOK_LT)    { return id; }
    else if (id == TOK_GT)    { return id; }
    else if (id == TOK_EQ)    { return id; }
    else if (id == TOK_NEQ)   { return id; }
    else if (id == TOK_AND)   { return id; }
    else if (id == TOK_OR)    { return id; }
    else if (id == TOK_XOR)   { return id; }
    return 0;
}

void expr() {
    if (term() == 0) { return; }
    next_token();
    int op = evalOp(tok);
    while (op != 0) {
        if (op == TOK_PLUS)       { g0(IRL_ADD);  next_term(); }
        else if (op == TOK_MINUS) { g0(IRL_SUB);  next_term(); }
        else if (op == TOK_STAR)  { g0(IRL_IMUL); next_term(); }
        else if (op == TOK_SLASH) { g0(IRL_IDIV); next_term(); }
        else if (op == TOK_LT)    { g0(IRL_LT);   next_term(); }
        else if (op == TOK_GT)    { g0(IRL_GT);   next_term(); }
        else if (op == TOK_EQ)    { g0(IRL_EQ);   next_term(); }
        else if (op == TOK_NEQ)   { g0(IRL_NEQ);  next_term(); }
        else if (op == TOK_AND)   { g0(IRL_AND);  next_term(); }
        else if (op == TOK_OR)    { g0(IRL_OR);   next_term(); }
        else if (op == TOK_XOR)   { g0(IRL_XOR);  next_term(); }
        else { syntax_error(); }
        next_token();
        op = evalOp(tok);
    }
}

void ifStmt() {
    g0(IRL_IF);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    expectToken(TOK_THEN);
    statement();
    if (tok == TOK_END) { next_token(); }
    else if (tok == ELSE_TOK) {
        g0(IRL_ELSE);
        next_token();
        statement();
        expectToken(TOK_END);
    }
}

void beginStmt() {
    g0(IRL_BEGIN);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    statements();
    if (tok == WHILE_TOK) { g0(IRL_WHILE); }
    else if (tok == TOK_UNTIL) { g0(IRL_UNTIL); }
    else if (tok == TOK_AGAIN) { g0(IRL_AGAIN); }
    else { syntax_error(); }
}

void returnStmt() {
    next_token();
    if (tok != TOK_SEMI) {
        expr();
    }
    expectToken(TOK_SEMI);
    g0(IRL_RET);
}

void intStmt() {
    char nm[32];
    nextShouldBe(TOK_ID);
    genSymbol(id_name, 'L');
    strcpy(nm, id_name);
    next_token();
    if (tok == TOK_SET) {
        next_token();
        expr();
        g0(IRL_IDSET);
    }
    expectToken(TOK_SEMI);
}

void idStmt() {
    int si = findSymbol(id_name, 'F');
    if (0 <= si) { g1(IRL_CALL, si); return; }
    if (si < 0) { si = findSymbol(id_name, 'I'); }
    if (si < 0) { si = findSymbol(id_name, 'C'); }
    if (si < 0) { msg(1, "variable not defined!"); }
    next_token();
    SYM_T *s = &symbols[si];
    if (tok == TOK_SET) { next_token(); expr(); g1(IRL_IDSET, si); }
    else { syntax_error(); }
    expectToken(TOK_SEMI);
}

void regStmt() {
    int regNum = int_val;
    next_token();
    if (tok == TOK_SET) { next_token(); expr(); g1(IRL_REGSET, regNum); }
    else { syntax_error(); }
    expectToken(TOK_SEMI);
}

void statements() {
    while (1) {
        if (tok == TOK_END) { return; }
        statement();
    }
}

void statement() {
    if (tok == IF_TOK)    { ifStmt(); }
    else if (tok == TOK_BEGIN) { beginStmt(); }
    else if (tok == RET_TOK)   { returnStmt(); }
    else if (tok == TOK_ID)    { idStmt(); }
    else if (tok == TOK_REG)   { regStmt(); }
    // else                       { expr(); expectToken(TOK_SEMI); }
}



/*---------------------------------------------------------------------------*/
// Code Generator

int addNext(int i, char *s) {
    int op = code[++i];
    int a1 = arg1[i];
    if (op == IRL_REG)      { printf(" [%c]", 'A' + a1); }
    else if (op == IRL_LIT) { printf(" %d", a1); }   
    else if (op == IRL_ID)  { printf(" [%s] ; %s", asmName(a1), symName(a1)); }   
    return i;
}

void genCode() {
    printf("; code: %d entries, %d used", CODE_SZ, codeSz);
    for (int i = 1; i <= codeSz; i++) {
        int op = code[i];
        int a1 = arg1[i];
        if (op == IRL_REG)        { printf("\n\tMOV  EAX, [%c]", 'A' + a1); }
        else if (op == IRL_NUM)   { printf("\n; %3d NUM   %d", i, a1); }
        else if (op == IRL_STR)   { printf("\n; %3d STR   %s", i, symName(a1)); }
        else if (op == IRL_LIT)   { printf("\n\tMOV  EAX, %d", a1); }
        else if (op == IRL_OP)    { printf("\n; %3d OP    %d", i, a1); }
        else if (op == IRL_LABEL) { printf("\n; %3d LABEL %d", i, a1); }
        else if (op == IRL_FUNC)  { printf("\n\n%s: ; %s", asmName(a1), symName(a1)); }
        else if (op == IRL_CALL)  { printf("\n; %3d CALL  %s", i, symName(a1)); }
        else if (op == IRL_RET)   { printf("\n\tRET"); }
        else if (op == IRL_JMP)   { printf("\n; %3d JMP   %d", i, a1); }
        else if (op == IRL_JMPZ)  { printf("\n; %3d JMPZ  %d", i, a1); }
        else if (op == IRL_ADD)   { printf("\n\tADD  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_SUB)   { printf("\n\tSUB  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_IMUL)  { printf("\n\tIMUL EAX,"); i = addNext(i, ""); }
        else if (op == IRL_IDIV)  { printf("\n\tIDIV");     i = addNext(i, ""); }
        else if (op == IRL_LT)    { printf("\n\tLT   EAX,"); i = addNext(i, ""); }
        else if (op == IRL_GT)    { printf("\n\tGT   EAX,"); i = addNext(i, ""); }
        else if (op == IRL_EQ)    { printf("\n\tEQ   EAX,"); i = addNext(i, ""); }
        else if (op == IRL_NEQ)   { printf("\n\tNEQ  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_AND)   { printf("\n\tAND  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_OR)    { printf("\n\tOR   EAX,"); i = addNext(i, ""); }
        else if (op == IRL_XOR)   { printf("\n\tXOR  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_SHL)   { printf("\n\tSHL  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_SHR)   { printf("\n\tSHR  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_INC)   { printf("\n\tINC  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_DEC)   { printf("\n\tDEC  EAX,"); i = addNext(i, ""); }
        else if (op == IRL_PLEQ)  { printf("\n; %3d PLEQ", i); }
        else if (op == IRL_IDSET) { printf("\n\tMOV  [%s], EAX; %s", asmName(a1), symName(a1)); }
        else if (op == IRL_REGSET){ printf("\n\tMOV  [%c], EAX", 'A' + a1); }
        else if (op == IRL_IF)    { printf("\n; %3d IF", i); }
        else if (op == IRL_ELSE)  { printf("\n; %3d ELSE", i); }
        else if (op == IRL_BEGIN) { printf("\n; %3d BEGIN", i); }
        else if (op == IRL_WHILE) { printf("\n; %3d WHILE", i); }
        else if (op == IRL_UNTIL) { printf("\n; %3d UNTIL", i); }
        else if (op == IRL_AGAIN) { printf("\n; %3d AGAIN", i); }
        else { printf("\n%3d ???   %d", i, op); }
    }
    printf("\n");
}

/*---------------------------------------------------------------------------*/
/* Top level definitions. */

void defSize(int type, int s) {
    if (tok == TOK_SEMI) { return; }
    expectToken(TOK_LARR);
    expectToken(TOK_NUM);
    expectToken(TOK_RARR);
}

int varDef(int type) {
    next_token();
    tokenShouldBe(TOK_ID);
    int s = genSymbol(id_name, type);
    next_token();
    defSize(type, s);
    expectToken(TOK_SEMI);
    return 0;
}

int funcDef() {
    next_token();
    int s = genSymbol(id_name, 'F');
    g1(IRL_FUNC, s);
    next_token();
    statements();
    expectToken(TOK_END);
    if (code[codeSz] != IRL_RET) { g0(IRL_RET); }
    return 0;
}

int parseDef() {
    if (tok == TOK_DEF) { return funcDef(); }
    if (tok == INT_TOK) { return varDef('I'); }
    if (tok == CHAR_TOK) { return varDef('C'); }
    return 0;
}

/*---------------------------------------------------------------------------*/
/* Main program. */
int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : "jn.jn";
    input_fp = stdin;
    if (fn) {
        input_fp = fopen(fn, "rt");
        if (!input_fp) { msg(1, "cannot open source file!"); }
    }
    // winLin('C');
    next_token();
    while (tok != EOI) { parseDef(); }
    if (input_fp) { fclose(input_fp); }
    // winLin('D');
    genCode();
    dumpSymbols();
    // winLin('I');
    return 0;
}
