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

int ch = ' ', tok, is_num, int_val;
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
    // printf("ln=%s", cur_line);
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
    start:
    id_name[0] = 0;
    is_num = 0;
    tok = 0;
    while (BTWI(ch, 1, 32)) { next_ch(); }
    if (ch == EOF) { return; }
    while (BTWI(ch, 33, 126)) {
        //printf("ch=[%c]\n", ch);
        id_name[tok++] = ch;
        next_ch();
    }
    id_name[tok] = 0;
    //printf("token=[%s]\n", id_name);
    if (strcmp(id_name, "//") == 0) { next_line(); goto start; }
}

int accept(char *str) {
    if (strcmp(id_name, str) == 0) { return 1; }
    return 0;
}

//---------------------------------------------------------------------------
// Symbols - 'I' = INT, 'C' = Char, 'P' = Parameter, 'L' = Local, 'S' = String, 'T' = Target
typedef struct { char type, name[23]; char asmName[8]; int sz; } SYM_T;
typedef struct { char name[32]; char *val; } STR_T;

SYM_T vars[500];
STR_T strings[500];
int numVars, numStrings;
int localOff, paramOff;

int findVar(char *name) {
    int i = numVars;
    while (0 < i) {
        if (strcmp(vars[i].name, name) == 0) { return i; }
        i = i-1;
    }
    return 0;
}


int addVar(char *name, char type) {
    if (strlen(name) > 20) { msg(1, "name too long"); }
    int i = ++numVars;
    SYM_T *x = &vars[i];
    x->type = type;
    x->sz = 4;
    strcpy(x->name, name);
    sprintf(x->asmName, "_v_%d", i);
    return i;
}

int addFunction(char *name) {
    return addVar(name, 'F');
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

void dumpSymbols() {
    printf("\n\n; symbols: %d entries, %d used\n", 100, numVars);
    printf("; num type size name\n");
    printf("; --- ---- ---- -----------------\n");
    for (int i = 1; i <= numVars; i++) {
        SYM_T *x = &vars[i];
        if (x->type == 'I') { printf("%-10s dd 0 ; %s\n", x->asmName, x->name); }
        if (x->type == 'C') { printf("%-10s db %d DUP(0) ; %s\n", x->asmName, x->sz, x->name); }
    }
    for (int i = 1; i <= numStrings; i++) {
        STR_T *x = &strings[i];
        printf("%-10s db \"%s\", 0\n", x->name, x->val);
    }
    printf("stk        rd 256\n");
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
    // int i = 1;
    // while (i <= here) {
    //     int op = opcodes[i];
    //     int a1 = arg1[i];
    //     int a2 = arg2[i];
    //     printf("\n; %3d: %-3d %-3d %-5d - ", i, opcodes[i], arg1[i], arg2[i] );
    //     // printf("\n%3d: %d %d %d %d - ", i, opcodes[i], arg1[i], arg2[i], arg3[i] );
    //     if (op == LOADVAR) { printf("LOADVAR %s, [%s]", regName(a1), genVarName(a2)); }
    //     if (op == LOADIMM) { printf("LOADIMM %s, %d",   regName(a1), a2); }
    //     if (op == LOADSTR) { printf("LOADSTR %s, [%s]", regName(a1), genVarName(a2)); }
    //     if (op == STORE)   { printf("STORE [%s], EAX",  genVarName(a1)); }
    //     if (op == PLEQ)    { printf("PLUSEQ [%s], EAX", genVarName(a1)); }
    //     if (op == DECVAR)  { printf("DECVAR [%s]",      genVarName(a1)); }
    //     if (op == INCVAR)  { printf("INCVAR [%s]",      genVarName(a1)); }
    //     if (op == ADD)     { printf("ADD %s, %s",       regName(a1), regName(a2)); }
    //     if (op == SUB)     { printf("SUB %s, %s",       regName(a1), regName(a2)); }
    //     if (op == MULT)    { printf("MULT %s, %s",      regName(a1), regName(a2)); }
    //     if (op == DIVIDE)  { printf("DIVIDE %s, %s",    regName(a1), regName(a2)); }
    //     if (op == LT)      { printf("CMP_LT %s, %s",    regName(a1), regName(a2)); }
    //     if (op == GT)      { printf("CMP_GT %s, %s",    regName(a1), regName(a2)); }
    //     if (op == EQ)      { printf("CMP_EQ %s, %s",    regName(a1), regName(a2)); }
    //     if (op == NEQ)     { printf("CMP_NEQ %s, %s",   regName(a1), regName(a2)); }
    //     if (op == DEF)     { printf("DEF %s",           vars[a1].name); }
    //     if (op == CALL)    { printf("CALL %s",          vars[a1].name); }
    //     if (op == PARAM)   { printf("PARAM EAX"); }
    //     if (op == RETURN)  { printf("RETURN"); }
    //     if (op == TARGET)  { printf("TARGET %s",        vars[a1].name); }
    //     if (op == JMP)     { printf("JMP %s",           vars[a1].name); }
    //     if (op == JMPZ)    { printf("JMPZ %s",          vars[a1].name); }
    //     i++;
    // }
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

char *varName(int i) { return vars[i].name; }
char *asmName(int i) { return vars[i].asmName; }

void genCode() {
    dumpIRL();
    int i = 1;
    while (i <= here) {
        int op = opcodes[i];
        int a1 = arg1[i];
        int a2 = arg2[i];
        char *SW = "\n\tXCHG ESP, EDI";
        // printf("\n; %3d: %-3d %-3d %-5d\n\t", i, op, a1, a2);
        if (op == LOADVAR) { printf("\n\tPUSH [%s]", asmName(a2)); }
        if (op == LOADIMM) { printf("\n\tPUSH %d", a2); }
        if (op == LOADSTR) { printf("\n\tLEA EAX, [%s]\n\tPUSH EAX", strings[a2].name); }
        if (op == STORE)   { printf("\n\tPOP [%s]", asmName(a1)); }
        if (op == PLEQ)    { printf("\n\tPOP EAX\n\tADD [%s], EAX", asmName(a1)); }
        if (op == DECVAR)  { printf("\n\tDEC [%s]", asmName(a1)); }
        if (op == INCVAR)  { printf("\n\tINC [%s]", asmName(a1)); }
        if (op == ADD)     { printf("\n\tPOP EAX\n\tADD [ESP], EAX"); }
        if (op == SUB)     { printf("\n\tPOP EAX\n\tSUB [ESP], EAX"); }
        if (op == MULT)    { printf("\n\tPOP EBX\n\tPOP EAX\n\tIMUL EAX, EBX\n\tPUSH EAX"); }
        if (op == DIVIDE)  { printf("\n\tPOP EBX\n\tPOP EAX\n\tCDQ\n\tIDIV EBX\n\tPUSH EAX"); }
        if (op == LT)      { printf("\n\tCMP_LT %s, %s", regName(a1), regName(a2)); }
        if (op == GT)      { printf("\n\tCMP_GT %s, %s", regName(a1), regName(a2)); }
        if (op == EQ)      { printf("\n\tCMP_EQ %s, %s", regName(a1), regName(a2)); }
        if (op == NEQ)     { printf("\n\tCMP_NEQ %s, %s", regName(a1), regName(a2)); }
        if (op == DEF)     { printf("\n%s:%s", vars[a1].name, SW); }
        if (op == CALL)    { printf("%s\n\tCALL %s%s", SW, vars[a1].name, SW); }
        if (op == PARAM)   { printf("\n\t; PARAM"); }
        if (op == RETURN)  { printf("%s\n\tRET", SW); }
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
        int s = addVar("exit", 'F');
        s = addVar("pv", 'I');
        P("format ELF executable");
        P("\n;================== code =====================");
        P("\nsegment readable executable");
        P("\nentry main");
        P("\n;================== library ==================");
        P("\ninit:\n\tLEA EDI, [stk]\n\tADD EDI, 1024\n\tRET\n\n");
        P("\nexit:\n\tMOV EAX, 1\n\tXOR EBX, EBX\n\tINT 0x80\n");
        P("\nputs:\n\tMOV [pv], EAX\n\tRET");
        P("\nputd:\n\tMOV [pv], EAX\n\tRET");
        P("\nputc:\n\tMOV [pv], EAX");
        P("\n\tMOV EAX, 4");
        P("\n\tMOV EBX, 0");
        P("\n\tLEA ECX, [pv]");
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

void ifStmt() {
}

void whileStmt() {
}

void statement() {
    tgtReg = 0;
    if (is_num)               { gen1(LOADIMM, int_val); }
    else if (accept("1+"))    { gen(INCVAR); }
    else if (accept("if"))    { ifStmt(); }
    else if (accept("begin")) { whileStmt(); }
    else if (accept("exit"))  { gen(RETURN); }
    else if (accept(";"))     { gen(RETURN); }
    else if (accept("1-"))    { gen(DECVAR); }
    else if (accept("+"))     { gen(ADD); }
    else if (accept("-"))     { gen(SUB); }
    else if (accept("*"))     { gen(MULT); }
    else if (accept("/"))     { gen(DIVIDE); }
    else if (accept("<"))     { gen(LT); }
    else if (accept(">"))     { gen(EQ); }
    else if (accept("="))     { gen(GT); }
    else { gen1(CALL, findVar(id_name)); }
}

void funcDef() {
    next_token();
    int s = addVar(id_name, 'F');
    gen1(DEF, s);
    while (1) {
        next_token();
        statement();
        if (accept(";")) { return; }
    }
}

void parseVar() {
    next_token();
    addVar(id_name, 'I');
}

void parseDef() {
    if (accept("var")) { parseVar(); }
    else if (accept(":")) { funcDef(); }
    else { statement(); }
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
    while (ch != EOF) { next_token(); parseDef(); }
    if (input_fp) { fclose(input_fp); }
    //optimizeIRL();
    genCode();
    winLin('D');
    dumpSymbols();
    winLin('I');
    return 0;
}
