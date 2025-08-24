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
#define G printf
#define P(str) printf("%s",str)

typedef unsigned char byte;

typedef struct { char type, name[32]; int sz; long val; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

// Tokens - NOTE: the first 8 must match the words list in tc.c
enum {
    DO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK, VOID_TOK, INT_TOK, CHAR_TOK, RET_TOK
    , TOK_LBRA, TOK_RBRA, TOK_LPAR, TOK_RPAR, TOK_LARR, TOK_RARR, TOK_COMMA
    , TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_INC, TOK_PLEQ
    , TOK_LT, TOK_EQ, TOK_GT, TOK_NEQ
    , TOK_SET, TOK_NUM, TOK_ID, TOK_FUNC, TOK_STR
    , TOK_OR, TOK_AND, TOK_XOR, TOK_LOR, TOK_LAND
    , TOK_SEMI, ND_VARINC, EOI
};

 /*---------------------------------------------------------------------------*/
 /* Lexer. */

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens
char *words[] = { "do", "else", "if" , "while", "void", "int", "char" , "return", NULL};

int ch = ' ', tok, int_val;
char id_name[256];
FILE *input_fp = NULL;
char cur_line[256] = {0};
int cur_off = 0, cur_lnum = 0, is_eof = 0;

void msg(int fatal, char *s, int cr, int ln) {
    printf("\n%s%s", s, cr ? "\n" : " ");
    if (ln) {
        printf("at(% d, % d)", cur_lnum, cur_off);
        printf("\n%s", cur_line);
        for (int i=2; i<cur_off; i++) { fprintf(stdout, " "); }
        printf("^\n");
    }
    if (fatal) { exit(1); }
}
void syntax_error() { msg(1, "syntax error", 0, 1); }

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
    case '+': next_ch(); tok = TOK_PLUS;
        if (ch == '+') { next_ch(); tok = TOK_INC; }
        if (ch == '=') { next_ch(); tok = TOK_PLEQ; }
        break;
    case '-': next_ch(); tok = TOK_MINUS; break;
    case '*': next_ch(); tok = TOK_STAR;  break;
    case '/': next_ch(); tok = TOK_SLASH;
        if (ch == '/') { // Line comment?
            while ((ch != 10) && (ch != EOF)) { next_ch(); }
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
                    tok = TOK_FUNC;
                    next_ch();
                    //if (ch == ')') { tok = TOK_FUNC; next_ch(); }
                    // else { syntax_error(); }
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
            msg(0,"name already defined with different type.", 1, 1);
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

void dumpSymbols() {
    printf("\n\n; symbols: %d entries, %d used\n", SYMBOLS_SZ, numSymbols);
    printf("; num type size name\n");
    printf("; --- ---- ---- -----------------\n");
    for (int i = 0; i < numSymbols; i++) {
        SYM_T *x = &symbols[i];
        if (x->type != 'F') { printf("%s:\tdb 0 DUP(%d)\n", x->name, x->sz); }
    }
}

/*---------------------------------------------------------------------------*/
/* Parser. */

void winLin(int seg) {
#ifdef _WIN32
    // Windows (32-bit)
    if (seg == 'C') {
        int s = genSymbol("exit", 'F');
        s = genSymbol("putc", 'F');
        s = genSymbol("_pc_buf", 'I');
        P("\nformat PE console");
        P("\ninclude 'win32ax.inc'");
        P("\n;================== code =====================");
        P("\n.code\nentry main");
        P("\n;================== library ==================");
        P("\nexit:\tRET\n");
        P("\nputc:\tRET\n");
        P("\n;=============================================");
    }
    else if (seg == 'D') {
        P("\n;================== data =====================");
        P("\n.data");
        P("\n;=============================================");
    }
#else
    // Linux (32-bit)
    if (seg == 'C') {
        int s = genSymbol("exit", 'F');
        s = genSymbol("putc", 'F');
        s = genSymbol("_pc_buf", 'I');
        P("\nformat ELF executable");
        P("\n;================== code =====================");
        P("\nsegment readable executable\nentry main");
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
#endif
}

int tgtReg = 0;
char regs[6][4] = { "EAX", "EBX", "ECX", "EDX", "ESI", "EDI" };
char *lReg, *rReg;
char *regName(int regNum) { return regs[regNum]; }

int statement();
int expr();
int term();

void parens() {
    //int tr = tgtReg - 1;
    //if (tr < 0) { tr = 0; P("; ** tr<0 **"); }
    expr();
    // G("\n\tMOV \t%s, %s", regName(tr), regName(tr+1));
    tokenShouldBe(TOK_RPAR);
}

int term() {
    if (tok == TOK_ID)  { G("\n\tMOV \t%s, [%s]", regName(tgtReg), id_name); return 1; }
    if (tok == TOK_NUM) { G("\n\tMOV \t%s, %d", regName(tgtReg), int_val); return 1; }
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
    return 0;
}

void doCmp(char *op) {
    G("\n\tCMP \t%s, %s", lReg, rReg);
    G("\n\tMOV \t%s, 0", lReg);
    G("\n\t%s \t@F", op);
    G("\n\tINC \t%s", lReg);
    G("\n@@:");
}

int expr() {
    if (term() == 0) { return 0; }
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
        else { syntax_error(); }
        next_token();
        op = evalOp(tok);
    }
    return 0;
}

int ifStmt() {
    static int iSeq = 1;
    G("\n\t; IF #%d ...", iSeq);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    G("\n\tTEST\tEAX, EAX");
    G("\n\tJNZ \tIF_%02d", iSeq);
    G("\n\t; Then #%d ...", iSeq);
    statement();
    G("\n\t; ENDIF #%d ...", iSeq);
    G("\nIF_%02d:", iSeq++);
    return 0;
}

int parseWhile() {
    expectNext(TOK_LPAR);
    expr();
    expectNext(TOK_RPAR);
    statement();
    return 0;
}

int parseReturn() {
    next_token();
    if (tok != TOK_SEMI) { expr(); }
    expectToken(TOK_SEMI);
    P("\n\tRET");
    return 0;
}

int intStmt() {
    nextShouldBe(TOK_ID);
    genSymbol(id_name, 'L');
    expectNext(TOK_SEMI);
    return 0;
}

void idStmt() {
    int s = findSymbol(id_name, 'L');
    if (s < 0) { s = findSymbol(id_name, 'I'); }
    if (s < 0) { syntax_error(); }
    next_token();
    if (tok == TOK_SET) {
        next_token();
        expr();
        G("\n\tMOV \t[%s], EAX", symbols[s].name);
    }
    expectToken(TOK_SEMI);
}

int statements() {
    while (1) {
        if (tok == TOK_RBRA) { next_token(); return 0; }
        statement();
    }
}

int statement() {
    tgtReg = 0;
    if (tok == TOK_LBRA) { next_token(); return statements(); }
    if (tok == IF_TOK)    { ifStmt(); return 0; }
    if (tok == WHILE_TOK) { return parseWhile(); }
    if (tok == RET_TOK)   { return parseReturn(); }
    if (tok == INT_TOK)   { return intStmt(); }
    if (tok == TOK_ID)    { idStmt(); return 0; }
    expr();
    expectToken(TOK_SEMI);
    return 0;
}

void defSize() {
    expectToken(TOK_LARR);
    expectToken(TOK_NUM);
    expectToken(TOK_RARR);
    expectToken(TOK_SEMI);
}

int funcDef() {
    P("\n;---------------------------------------------");
    G("\n%s:", id_name);
    int s = genSymbol(id_name, 'F');
    expectNext(TOK_RPAR);
    expectToken(TOK_LBRA);
    return statements();
}

int parseVar(int type) {
    next_token();
    if (tok == TOK_FUNC) { return funcDef(); }
    tokenShouldBe(TOK_ID);
    int s = genSymbol(id_name, type);
    next_token();
    defSize(type, s);
    return 0;
}

int parseDef() {
    if (tok == VOID_TOK) { next_token(); tokenShouldBe(TOK_FUNC); return funcDef(); }
    if (tok == INT_TOK) { return parseVar('I'); }
    if (tok == CHAR_TOK) { return parseVar('C'); }
    syntax_error();
    return 0;
}

void defs() {
    next_token();
    while (tok != EOI) {
        parseDef();
    }
}

/*---------------------------------------------------------------------------*/
/* Main program. */

int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : NULL;
    if (fn) { input_fp = fopen(fn, "rt"); }
    if (!input_fp) { input_fp = stdin; }
    hInit(0);
    //gInit();
    P("; TC source file: ");
    P(fn ? fn : "stdin");
    winLin('C');
    defs();
    if (input_fp != stdin) { fclose(input_fp); }
    // printf("; %d lines, %d nodes\n", gHere, num_nodes);

    winLin('D');
    dumpSymbols();

    return 0;
}
