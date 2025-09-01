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
    printf("\n%s at(%d, %d)", s, cur_lnum, cur_off);
    printf("\n%s", cur_line);
    for (int i = 2; i < cur_off; i++) { printf(" "); } printf("^");
    if (fatal) { fprintf(stderr, "\n%s (see output for details)\n", s); exit(1); }
}
void syntax_error() { msg(1, "syntax error"); }

char* regName(int regNum) { return regs[regNum]; }
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
void tokenShouldNotBe(int x) { if (tok == x) { syntax_error(); } }

//---------------------------------------------------------------------------
// Symbols - 'I' = INT, 'C' = Char, 'P' = Parameter, 'L' = Local, 'S' = String, 'T' = Target
typedef struct { char type, name[32]; int sz; int offset; } SYM_T;
typedef struct { char name[32]; int numParams; } FUNC_T;
typedef struct { char name[32]; char *val; } STR_T;

FUNC_T functions[1000];
SYM_T vars[1000];
STR_T strings[1000];
int numVars, numFunctions, numStrings;
int localOff, paramOff;

char *varName(int sym, int from) { return vars[sym].name; }

int findFunction(char *name) {
    int i = numFunctions;
    while (0 <= i) {
        if (strcmp(functions[i].name, name) == 0) { return i; }
        i = i - 1;
    }
    return 0;
}

int addFunction(char *name) {
    int i = ++numFunctions;
    strcpy(functions[i].name, name);
    functions[i].numParams = 0;
    return i;
}

int findVar(char *name) {
    int i = numVars;
    while (0 < i) {
        if (strcmp(vars[i].name, name) == 0) { return i; }
        i = i-1;
    }
    return 0;
}


int addVar(char *name, char type) {
    int i = ++numVars;
    SYM_T *x = &vars[i];
    x->type = type;
    x->sz = 4;
    strcpy(x->name, name);
    if (type == 'L') { localOff += 4; x->offset = localOff; }
    else if (type == 'P') { paramOff += 4; x->offset = paramOff; }
    return i;
}

int genTargetSymbol() {
    static char name[8];
    static int seq = 0;
    sprintf(name, ".t%d", ++seq);
    return addVar(name, 'T');
}

int addString(char *str) {
    int i = ++numStrings;
    sprintf(strings[i].name, "_str%d", i);
    strings[i].val = hAlloc(strlen(str) + 1);
    strcpy(strings[i].val, str);
    return i;
}

void InitLocals() {
    localOff = 0;
    paramOff = 4;
}

void forgetLocals() {
    InitLocals();
    int i = numVars;
    while (0 < i) {
        int t = vars[i].type;
        if ((t == 'C') || (t == 'I')) {
            numVars = i;
            return;
        }
        i = i - 1;
    }
    numVars = 0;
}

char *genVarName(int num) {
    static char ret[32];
    SYM_T *s = &vars[num];
    if (s->type == 'L') { sprintf(ret, "EBP-%d", s->offset); return ret; }
    if (s->type == 'P') { sprintf(ret, "EBP+%d", s->offset); return ret; }
    return s->name;
}

void dumpSymbols() {
    printf("\n\n; symbols: %d entries, %d used\n", 100, numVars);
    printf("; num type size name\n");
    printf("; --- ---- ---- -----------------\n");
    for (int i = 1; i <= numVars; i++) {
        SYM_T *x = &vars[i];
        if (x->type == 'I') { printf("%-10s\tdd 0\n", x->name); }
        if (x->type == 'C') { printf("%-10s\tdb %d DUP(0)\n", x->name, x->sz); }
    }
    for (int i = 1; i <= numStrings; i++) {
        STR_T *x = &strings[i];
        printf("%-10s\tdb \"%s\", 0\n", x->name, x->val);
    }
}

//---------------------------------------------------------------------------
// IRL
enum { NOTHING, LOADVAR, LOADIMM, LOADSTR, STORE
    , ADD, SUB, MULT, DIVIDE
    , AND, OR, XOR
    , JMP, JMPZ, JMPNZ, TARGET
    , DEF, CALL, PARAM, RETURN
    , LT, GT, EQ, NEQ
    , PLEQ, DECVAR, INCVAR
};

int opcodes[10000], here;
int arg1[10000];
int arg2[10000];

void gInit() { here = 0; }
void gen(int op) { ++here; opcodes[here] = op; }
void setA1(int h, int v)   { arg1[h?h:here] = v; }
void setA2(int h, int v)   { arg2[h?h:here] = v; }
void gen1(int op, int a1) { gen(op); setA1(here, a1); }
void gen2(int op, int a1, int a2) { gen1(op, a1); setA2(here, a2); }

void dumpIRL() {
    int i = 1;
    while (i <= here) {
        int op = opcodes[i];
        int a1 = arg1[i];
        int a2 = arg2[i];
        printf("\n; %3d: %-3d %-3d %-5d - ", i, opcodes[i], arg1[i], arg2[i] );
        // printf("\n%3d: %d %d %d %d - ", i, opcodes[i], arg1[i], arg2[i], arg3[i] );
        if (op == LOADVAR) { printf("LOADVAR %s, [%s]", regName(a1), genVarName(a2)); }
        if (op == LOADIMM) { printf("LOADIMM %s, %d",   regName(a1), a2); }
        if (op == LOADSTR) { printf("LOADSTR %s, [%s]", regName(a1), genVarName(a2)); }
        if (op == STORE)   { printf("STORE [%s], EAX",  genVarName(a1)); }
        if (op == PLEQ)    { printf("PLUSEQ [%s], EAX", genVarName(a1)); }
        if (op == DECVAR)  { printf("DECVAR [%s]",      genVarName(a1)); }
        if (op == INCVAR)  { printf("INCVAR [%s]",      genVarName(a1)); }
        if (op == ADD)     { printf("ADD %s, %s",       regName(a1), regName(a2)); }
        if (op == SUB)     { printf("SUB %s, %s",       regName(a1), regName(a2)); }
        if (op == MULT)    { printf("MULT %s, %s",      regName(a1), regName(a2)); }
        if (op == DIVIDE)  { printf("DIVIDE %s, %s",    regName(a1), regName(a2)); }
        if (op == LT)      { printf("CMP_LT %s, %s",    regName(a1), regName(a2)); }
        if (op == GT)      { printf("CMP_GT %s, %s",    regName(a1), regName(a2)); }
        if (op == EQ)      { printf("CMP_EQ %s, %s",    regName(a1), regName(a2)); }
        if (op == NEQ)     { printf("CMP_NEQ %s, %s",   regName(a1), regName(a2)); }
        if (op == DEF)     { printf("DEF %s",           functions[a1].name); }
        if (op == CALL)    { printf("CALL %s",          functions[a1].name); }
        if (op == PARAM)   { printf("PARAM EAX"); }
        if (op == RETURN)  { printf("RETURN"); }
        if (op == TARGET)  { printf("TARGET %s",        vars[a1].name); }
        if (op == JMP)     { printf("JMP %s",           vars[a1].name); }
        if (op == JMPZ)    { printf("JMPZ %s",          vars[a1].name); }
        i++;
    }
}

void optimizeIRL() {
    int i = 1;
    while (i <= here) {
        int op = opcodes[i];
        int a1 = arg1[i];
        int a2 = arg2[i];
        //if ((op == CALL) && (opcodes[i + 1] == RETURN)) {
        //    opcodes[i] = JMP;
        //    opcodes[i + 1] = NOTHING;
        //}
        i++;
    }
}

void genCode() {
    dumpIRL();
    int i = 1;
    while (i <= here) {
        int op = opcodes[i];
        int a1 = arg1[i];
        int a2 = arg2[i];
        // printf("\n; %3d: %-3d %-3d %-5d\n\t", i, op, a1, a2);
        if (op == LOADVAR) { printf("\n\tPUSH [%s]", genVarName(arg2[i])); }
        if (op == LOADIMM) { printf("\n\tPUSH %d", a2); }
        if (op == LOADSTR) { printf("\n\tLEA EAX, [%s]\n\tPUSH EAX", strings[a2].name); }
        if (op == STORE)   { printf("\n\tPOP DWORD [%s]", genVarName(a1)); }
        if (op == PLEQ)    { printf("\n\tPOP EAX\n\tADD DWORD [%s], EAX", genVarName(a1)); }
        if (op == DECVAR)  { printf("\n\tDEC DWORD [%s]", genVarName(a1)); }
        if (op == INCVAR)  { printf("\n\tINC DWORD [%s]", genVarName(a1)); }
        if (op == ADD)     { printf("\n\tPOP EAX\n\tADD [ESP], EAX"); }
        if (op == SUB)     { printf("\n\tPOP EAX\n\tSUB [ESP], EAX"); }
        if (op == MULT)    { printf("\n\tPOP EBX\n\tPOP EAX\n\tIMUL EAX, EBX\n\tPUSH EAX"); }
        if (op == DIVIDE)  { printf("\n\tPOP EBX\n\tPOP EAX\n\tCDQ\n\tIDIV EBX\n\tPUSH EAX"); }
        if (op == LT)      { printf("\n\tCMP_LT %s, %s", regName(a1), regName(a2)); }
        if (op == GT)      { printf("\n\tCMP_GT %s, %s", regName(a1), regName(a2)); }
        if (op == EQ)      { printf("\n\tCMP_EQ %s, %s", regName(a1), regName(a2)); }
        if (op == NEQ)     { printf("\n\tCMP_NEQ %s, %s", regName(a1), regName(a2)); }
        if (op == DEF)     { printf("\n%s:", functions[a1].name); }
        if (op == CALL)    { printf("\n\tCALL %s", functions[a1].name); }
        if (op == PARAM)   { printf("\n\t; PARAM"); }
        if (op == RETURN)  { printf("\n\tRET"); }
        if (op == TARGET)  { printf("\n%s:", vars[a1].name); }
        if (op == JMP)     { printf("\n\tJMP %s", vars[a1].name); }
        if (op == JMPZ)    { printf("\n\tJMPZ %s", vars[a1].name); }
        i++;
    }
}

//---------------------------------------------------------------------------
// Parser / code generator.
void winLin(int seg) {
#ifdef _WIN32
    // Windows (32-bit)
    if (seg == 'C') {
        int s = addFunction("exit");
        s = addFunction("puts");
        s = addFunction("putc");
        s = addFunction("putd");
        s = addVar("pv", 'I');
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
        int s = addFunction("exit");
        s = addVar("pv", 'I');
        s = addVar("_pc_buf", 'I');
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

void parens() { expr(); tokenShouldBe(TOK_RPAR); }

int term() {
    if (tok == TOK_ID) { gen2(LOADVAR, tgtReg, findVar(id_name)); return 1; }
    if (tok == TOK_NUM) { gen2(LOADIMM, tgtReg, int_val); return 1; }
    if (tok == TOK_STR) { gen2(LOADSTR, tgtReg, addString(id_name)); return 1; }
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

void doCmp(int op) {
    opPrep();
}

void expr() {
    if (term() == 0) { return; }
    next_token();
    int op = evalOp(tok);
    while (op != 0) {
        if (op == TOK_PLUS)       { opPrep(); gen2(ADD,    tgtReg, tgtReg+1); }
        else if (op == TOK_MINUS) { opPrep(); gen2(SUB,    tgtReg, tgtReg+1); }
        else if (op == TOK_STAR)  { opPrep(); gen2(MULT,   tgtReg, tgtReg+1); }
        else if (op == TOK_SLASH) { opPrep(); gen2(DIVIDE, tgtReg, tgtReg+1); }
        else if (op == TOK_AND)   { opPrep(); gen2(AND,    tgtReg, tgtReg+1); }
        else if (op == TOK_OR)    { opPrep(); gen2(OR,     tgtReg, tgtReg+1); }
        else if (op == TOK_XOR)   { opPrep(); gen2(XOR,    tgtReg, tgtReg+1); }
        else if (op == TOK_LT)    { opPrep(); gen2(LT,     tgtReg, tgtReg+1); }
        else if (op == TOK_GT)    { opPrep(); gen2(GT,     tgtReg, tgtReg+1); }
        else if (op == TOK_EQ)    { opPrep(); gen2(EQ,     tgtReg, tgtReg+1); }
        else if (op == TOK_NEQ)   { opPrep(); gen2(NEQ,    tgtReg, tgtReg+1); }
        else { syntax_error(); }
        next_token();
        op = evalOp(tok);
    }
}

void ifStmt() {
    static int iSeq = 1;
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    int tgt = genTargetSymbol();
    gen1(JMPZ, tgt);
    statement();
    gen1(TARGET, tgt);
}

void whileStmt() {
    int t1 = genTargetSymbol();
    int t2 = genTargetSymbol();
    gen1(TARGET, t1);
    expectNext(TOK_LPAR);
    expr();
    expectToken(TOK_RPAR);
    gen1(JMPZ, t2);
    statement();
    gen1(JMP, t1);
    gen1(TARGET, t2);
}

void returnStmt() {
    next_token();
    if (tok != TOK_SEMI) {
        expr();
    }
    expectToken(TOK_SEMI);
    gen(RETURN);
}

void intStmt() {
    char nm[32];
    nextShouldBe(TOK_ID);
    int si = addVar(id_name, 'L');
    strcpy(nm, id_name);
    next_token();
    if (tok == TOK_SET) {
        next_token();
        expr();
        gen1(STORE, si);
    }
    expectToken(TOK_SEMI);
}

void idStmt() {
    int si = findVar(id_name);
    if (si < 0) { msg(1, "variable not defined!"); }
    next_token();
    if (tok == TOK_SET) { next_token(); expr(); gen1(STORE, si); }
    else if (tok == TOK_PLEQ) { next_token(); expr(); genVarName(si); gen1(PLEQ, si); }
    else if (tok == TOK_DEC)  { next_token(); gen1(DECVAR, si); }
    else if (tok == TOK_INC)  { next_token(); gen1(INCVAR, si); }
    else { syntax_error(); }
    expectToken(TOK_SEMI);
}

void funcStmt() {
    int si = findFunction(id_name);
    if (si == 0) { si = addFunction(id_name); }
    next_token();
    while (tok != TOK_RPAR) {
        expr();
        gen(PARAM);
        if (tok == TOK_COMMA) { next_token(); tokenShouldNotBe(TOK_RPAR); }
    }
    gen1(CALL, si);
    expectToken(TOK_RPAR);
    expectToken(TOK_SEMI);
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
    else if (tok == TOK_FUNC)  { funcStmt(); }
    else                       { expr(); expectToken(TOK_SEMI); }
}

void defSize(int type, int s) {
    if (tok == TOK_SEMI) { return; }
    expectToken(TOK_LARR);
    expectToken(TOK_NUM);
    expectToken(TOK_RARR);
}

int funcDef() {
    int s = addFunction(id_name);
    gen1(DEF, s);
    InitLocals();
    next_token();
    while (tok == INT_TOK) {
        nextShouldBe(TOK_ID);
        functions[s].numParams++;
        addVar(id_name,'P');
        next_token();
        if (tok == TOK_COMMA) { next_token(); }
    }
    expectToken(TOK_RPAR);
    expectToken(TOK_LBRA);
    statements();
    gen(RETURN);
    optimizeIRL();
    genCode();
    here = 0;
    forgetLocals();
    printf("\n");
    return 0;
}

int parseVar(int type) {
    next_token();
    if (tok == TOK_FUNC) { return funcDef(); }
    tokenShouldBe(TOK_ID);
    int s = addVar(id_name, type);
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
