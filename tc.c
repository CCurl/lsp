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

typedef struct { char type, name[32]; int sz; char *val; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

// Tokens - NOTE: the first 8 must match the words list in tc.c
enum {
    NO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK, VOID_TOK, INT_TOK, CHAR_TOK, RET_TOK
    , TOK_LBRA, TOK_RBRA, TOK_LPAR, TOK_RPAR, TOK_LARR, TOK_RARR, TOK_COMMA
    , TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_INC, TOK_DEC, TOK_PLEQ
    , TOK_LT, TOK_EQ, TOK_GT, TOK_NEQ
    , TOK_SET, TOK_NUM, TOK_ID, TOK_FUNC, TOK_STR
    , TOK_OR, TOK_AND, TOK_XOR, TOK_LOR, TOK_LAND
    , TOK_SEMI, EOI
};

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens
char *words[] = { "", "else", "if" , "while", "void", "int", "char" , "return", NULL};

int ch = ' ', tok, int_val;
char id_name[256];
FILE *input_fp = NULL;
char cur_line[256] = {0};
int cur_off = 0, cur_lnum = 0, is_eof = 0;
int tgtReg = 0, tok_len;
char regs[6][4] = { "EAX", "EBX", "ECX", "EDX", "ESI", "EDI" };
char *lReg, *rReg;

void statement();
void expr();
int term();

void msg(int fatal, char *s) {
    printf("\n; %s at(%d, %d)", s, cur_lnum, cur_off);
    printf("\n; %s", cur_line);
    for (int i = 1; i < cur_off; i++) { printf(" "); } printf("^");
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
    int l = strlen(cur_line);
    if ((0 < l) && (cur_line[l-1] == 10)) { cur_line[l-1] = 0; }
    else { l++; }
    printf("\n\t; %s", cur_line);
    cur_line[l-1] = 10;
    cur_line[l] = 0;
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
    case '{': next_ch(); tok = TOK_LBRA;  break;
    case '}': next_ch(); tok = TOK_RBRA;  break;
    case '(': next_ch(); tok = TOK_LPAR;  break;
    case ')': next_ch(); tok = TOK_RPAR;  break;
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
            tok = 0;
            while ((words[tok] != NULL) && (strcmp(words[tok], id_name) != 0)) { tok++; }
            if (words[tok] == NULL) {
                tok = TOK_ID;
                if (ch == '(') {
                    tok = TOK_FUNC;
                    next_ch();
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

/*---------------------------------------------------------------------------*/
/* Symbols */
SYM_T symbols[SYMBOLS_SZ];
int numSymbols = 0;

char *symName(int sym) { return symbols[sym].name; }

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
    x->val = 0;
    x->type = type;
    x->sz = 4;
    strcpy(x->name, name);
    return i;
}

char *genStringSymbol(char *str) {
    static char name[32];
    static int strNum = 0;
    sprintf(name, "_s%03d_", ++strNum);
    SYM_T *s = &symbols[genSymbol(name, 'S')];
    s->val = hAlloc(strlen(str)+1);
    strcpy((char *)s->val, str);
    return name;
}

void dumpSymbols() {
    printf("\n\n; symbols: %d entries, %d used\n", SYMBOLS_SZ, numSymbols);
    printf("; num type size name\n");
    printf("; --- ---- ---- -----------------\n");
    for (int i = 0; i < numSymbols; i++) {
        SYM_T *x = &symbols[i];
        if (x->type == 'S') { printf("%-10s\tdb \"%s\",0\n", x->name, x->val); }
        else if (x->type == 'I') { printf("%-10s\tdd 0\n", x->name); }
        else if (x->type == 'L') { printf("%-10s\tdd 0\n", x->name); }
    }
}

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
        P("\nformat PE console");
        P("\ninclude 'win32ax.inc'\n");
        P("\n; ======================================= ");
        P("\nsection '.code' code readable executable");
        P("\n;=======================================*/");
        P("\n\nstart: JMP main");
        P("\n;================== library ==================");
        P("\nexit:\tRET\n");
        P("\n\tputs:\tcinvoke printf, \"%s\", [pv]");
        P("\n\tRET\n");
        P("\n\tputc:\tcinvoke printf, \"%c\", [pv]");
        P("\n\tRET\n");
        P("\n\tputd:\tcinvoke printf, \"%d\", [pv]");
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
        P("\nimport msvcrt,printf,'printf',scanf,'scanf',getch,'_getch'\n");
    }
#else
    // Linux (32-bit)
    if (seg == 'C') {
        int s = genSymbol("exit", 'F');
        s = genSymbol("putc", 'F');
        s = genSymbol("_pc_buf", 'I');
        P("\nformat ELF executable");
        P("\n;================== code =====================");
        P("\nsegment readable executable");
        P("\nentry main");
        P("\n;================== library ==================");
        P("\nexit:\n\tMOV EAX, 1\n\tXOR EBX, EBX\n\tINT 0x80\n");
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
    if (tok == TOK_ID)   { G("\n\tMOV \t%s, [%s]", regName(tgtReg), id_name); return 1; }
    if (tok == TOK_NUM)  { G("\n\tMOV \t%s, %d",   regName(tgtReg), int_val); return 1; }
    if (tok == TOK_STR)  { G("\n\tLEA \t%s, [%s]", regName(tgtReg), genStringSymbol(id_name)); return 1; }
    if (tok == TOK_LPAR) { next_token();  parens();  return 1; }
    return 0;
}

void opPrep() {
    ++tgtReg;
    next_token();
    term();
    rReg = regName(tgtReg--);
    lReg = regName(tgtReg);
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

void doCmp(char *op) {
    G("\n\tCMP \t%s, %s", lReg, rReg);
    G("\n\tMOV \t%s, 0", lReg);
    G("\n\t%s \t@F", op);
    G("\n\tDEC \t%s", lReg);
    G("\n@@:");
}

void expr() {
    if (term() == 0) { return; }
    next_token();
    int op = evalOp(tok);
    while (op != 0) {
        if (op == TOK_PLUS) { opPrep(); G("\n\tADD \t%s, %s", lReg, rReg); }
        else if (op == TOK_MINUS) { opPrep(); G("\n\tSUB \t%s, %s", lReg, rReg); }
        else if (op == TOK_STAR)  { opPrep(); G("\n\tIMUL\t%s, %s", lReg, rReg); }
        else if (op == TOK_SLASH) { opPrep();
            if (tgtReg != 0) { G("\n\tXCHG\tEAX, %s", lReg); }
            if (tgtReg != 1) { G("\n\tXCHG\tECX, %s", rReg); }
            G("\n\tCDQ\n\tIDIV\tECX");
            if (tgtReg != 0) { G("\n\tXCHG\tEAX, %s", lReg); }
        }
        else if (op == TOK_LT)  { opPrep(); doCmp("JGE"); }
        else if (op == TOK_GT)  { opPrep(); doCmp("JLE"); }
        else if (op == TOK_EQ)  { opPrep(); doCmp("JNE"); }
        else if (op == TOK_NEQ) { opPrep(); doCmp("JE"); }
        else if (op == TOK_AND) { opPrep(); G("\n\tAND \t%s, %s", lReg, rReg); }
        else if (op == TOK_OR)  { opPrep(); G("\n\tOR  \t%s, %s", lReg, rReg); }
        else if (op == TOK_XOR) { opPrep(); G("\n\tXOR \t%s, %s", lReg, rReg); }
        else { syntax_error(); }
        next_token();
        op = evalOp(tok);
    }
}

void ifStmt() {
    static int iSeq = 1;
    G("\nIF_%02d:", iSeq);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    G("\n\tTEST\tEAX, EAX");
    G("\n\tJZ  \tENDIF_%02d", iSeq);
    G("\nTHEN_%02d:", iSeq);
    statement();
    G("\nENDIF_%02d:", iSeq++);
}

void whileStmt() {
    static int iSeq = 1;
    G("\nWHILE_%02d:", iSeq);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    G("\n\tTEST\tEAX, EAX");
    G("\n\tJZ  \tWEND_%02d", iSeq);
    statement();
    G("\n\tJMP \tWHILE_%02d", iSeq);
    G("\nWEND_%02d:", iSeq++);
}

void returnStmt() {
    next_token();
    if (tok != TOK_SEMI) {
        expr();
        // next_token();
    }
    expectToken(TOK_SEMI);
    P("\n\tRET");
}

void intStmt() {
    nextShouldBe(TOK_ID);
    genSymbol(id_name, 'L');
    expectNext(TOK_SEMI);
}

void idStmt() {
    int si = findSymbol(id_name, 'L');
    if (si < 0) { si = findSymbol(id_name, 'I'); }
    if (si < 0) { syntax_error(); }
    next_token();
    SYM_T *s = &symbols[si];
    if (tok == TOK_SET) { next_token(); expr(); G("\n\tMOV \t[%s], EAX", s->name); }
    else if (tok == TOK_PLEQ) { next_token(); expr(); G("\n\tADD \t[%s], EAX", s->name); }
    else if (tok == TOK_DEC)  { next_token(); G("\n\tDEC \t[%s]", s->name); }
    else if (tok == TOK_INC)  { next_token(); G("\n\tINC \t[%s]", s->name); }
    else { syntax_error(); }
    expectToken(TOK_SEMI);
}

void funcStmt() {
    expectNext(TOK_RPAR);
    expectToken(TOK_SEMI);
    G("\n\tCALL\t%s", id_name);
}

void statements() {
    while (1) {
        if (tok == TOK_RBRA) { next_token(); return; }
        statement();
    }
}

void statement() {
    tgtReg = 0;
    if (tok == TOK_LBRA)       { next_token(); statements(); }
    else if (tok == IF_TOK)    { ifStmt(); }
    else if (tok == WHILE_TOK) { whileStmt(); }
    else if (tok == RET_TOK)   { returnStmt(); }
    else if (tok == INT_TOK)   { intStmt(); }
    else if (tok == TOK_ID)    { idStmt(); }
    else if (tok == TOK_FUNC)  { expectNext(TOK_RPAR); G("\n\tCALL\t%s", id_name); }
    else                       { expr(); expectToken(TOK_SEMI); }
}

void defSize() {
    if (tok == TOK_SEMI) { return; }
    expectToken(TOK_LARR);
    expectToken(TOK_NUM);
    expectToken(TOK_RARR);
}

int funcDef() {
    P("\n;---------------------------------------------");
    G("\n%s:", id_name);
    int s = genSymbol(id_name, 'F');
    expectNext(TOK_RPAR);
    expectToken(TOK_LBRA);
    statements();
    P("\n\tRET");
    return 0;
}

int parseVar(int type) {
    next_token();
    if (tok == TOK_FUNC) { return funcDef(); }
    tokenShouldBe(TOK_ID);
    int s = genSymbol(id_name, type);
    next_token();
    defSize(type, s);
    expectToken(TOK_SEMI);
    return 0;
}

int parseDef() {
    if (tok == VOID_TOK) { nextShouldBe(TOK_FUNC); return funcDef(); }
    if (tok == INT_TOK) { return parseVar('I'); }
    if (tok == CHAR_TOK) { return parseVar('C'); }
    syntax_error();
    return 0;
}

/*---------------------------------------------------------------------------*/
/* Main program. */
int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : NULL;
    input_fp = stdin;
    if (fn) {
        input_fp = fopen(fn, "rt");
        if (!input_fp) { msg(1, "cannot open source file!"); }
    }
    winLin('C');
    next_token();
    while (tok != EOI) { parseDef(); }
    if (input_fp) { fclose(input_fp); }
    winLin('D');
    dumpSymbols();
    winLin('I');
    return 0;
}
