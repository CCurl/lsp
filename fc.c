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

//---------------------------------------------------------------------------
// Tokens
int ch = ' ', tok, is_num, digit, int_val;
FILE *input_fp = NULL;
char token[32], cur_line[256] = {0};
int is_eof = 0, cur_lnum, cur_off;
int stk[64], sp=0;

void push(int x) { stk[++sp] = x; }
int  pop() { return stk[sp--]; }

void statement();

void msg(int fatal, char *s) {
    printf("\n%s at(%d, %d)", s, cur_lnum, cur_off);
    printf("\n%s", cur_line);
    for (int i = 2; i < cur_off; i++) { printf(" "); } printf("^");
    if (fatal) { fprintf(stderr, "\n%s (see output for details)\n", s); exit(1); }
}
void syntax_error() { msg(1, "syntax error"); }

void next_line() {
    cur_off = 0;
    cur_lnum++;
    if (fgets(cur_line, 256, input_fp) != cur_line) {
        is_eof = 1;
    }
    // int l = strlen(cur_line);
    // if ((0 < l) && (cur_line[l-1] == 10)) { cur_line[l-1] = 0; }
    // else { l++; }
    // printf("ln=%s", cur_line);
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

int strEq(char *s1, char *s2) { return (strcmp(s1, s2) == 0) ? 1 : 0; }
int accept(char *str) { return strEq(token, str); }

int isDigit(char c, int b) {
    if ((b == 2)  && (BTWI(c, '0','1'))) { digit = c - '0'; return 1; }
    if ((b == 8)  && (BTWI(c, '0','7'))) { digit = c - '0'; return 1; }
    if ((b == 10) && (BTWI(c, '0','9'))) { digit = c - '0'; return 1; }
    if (b == 16) {
        if (BTWI(c, '0','9')) { digit = c - '0'; return 1; }
        if (BTWI(c, 'A','F')) { digit = c - 'A' + 10; return 1; }
        if (BTWI(c, 'a','f')) { digit = c - 'a' + 10; return 1; }
    }
    return 0; 
}

int checkNumber(char *w, int base) {
    int_val = 0;
    if ((w[0] == '\'') && (w[2] == w[0]) && (w[3] == 0)) { int_val = w[1]; return 1; }
    if (*w == '%') { ++w; base = 2; }
    else if (*w == 'o') { ++w; base = 8; }
    else if (*w == '#') { ++w; base = 10; }
    else if (*w == '$') { ++w; base = 16; }
    if (*w == 0) { return 0; }
    while (*w) {
        if (isDigit(*(w++), base) == 0) { return 0; }
        int_val = (int_val* base) + digit;
    }
    return 1;
}

void next_token() {
    start:
    token[0] = 0;
    tok = 0;
    while (BTWI(ch, 1, 32)) { next_ch(); }
    if (ch == EOF) { return; }
    while (BTWI(ch, 33, 126)) {
        token[tok++] = ch;
        next_ch();
    }
    token[tok] = 0;
    if (accept("//")) { next_line(); goto start; }
    is_num = checkNumber(token, 10);
    // printf("\n; -%s/%d/%d-", token, is_num, int_val);
}

//---------------------------------------------------------------------------
// Symbols - 'I' = INT, 'F' = Function, 'S' = String, 'T' = Target
typedef struct { char type, name[23]; char asmName[8]; int sz; } SYM_T;
typedef struct { char name[32]; char *val; } STR_T;

SYM_T vars[500];
STR_T strings[500];
int numVars, numStrings;

int  varType(int i)  { return vars[i].type; }
char *varName(int i) { return vars[i].name; }
char *asmName(int i) { return vars[i].asmName; }

int findVar(char *name, char type) {
    int i = numVars;
    while (0 < i) {
        if (strEq(varName(i), name)) {
            if ((varType(i) == type)) { return i; }
        }
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
    sprintf(x->asmName, "%c%d", type, i);
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
    sprintf(strings[i].name, "S%d", i);
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
    printf("rstk       rd 256\n");
}

//---------------------------------------------------------------------------
// IRL
enum { NOTHING, VARADDR, LIT, LOADSTR, STORE, FETCH
    , ADD, SUB, MULT, DIVIDE
    , DROP, SWAP, OVER
    , AND, OR, XOR
    , JMP, JMPZ, JMPNZ, TARGET
    , DEF, CALL, PARAM, RETURN
    , LT, GT, EQ, NEQ
    , PLEQ, DECTOS, INCTOS
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

void genStartupCode() {
    printf("\ninit:\n\tLEA EBP, [rstk]\n\tRET\n");
    printf("\n; Move the return addr to the [EBP] stack");
    printf("\n; NB: EDX is destroyed");
    printf("\nRETtoEBP:");
    printf("\n\tPOP  EDX");
    printf("\n\tADD  EBP, 4");
    printf("\n\tPOP  DWORD [EBP]");
    printf("\n\tPUSH EDX");
    printf("\n\tRET\n");
    printf("\n; Restore the return addr from the [EBP] stack");
    printf("\n; NB: EDX is destroyed");
    printf("\nEBPtoRET:");
    printf("\n\tPOP  EDX");
    printf("\n\tPUSH DWORD [EBP]");
    printf("\n\tSUB  EBP, 4");
    printf("\n\tPUSH EDX");
    printf("\n\tRET\n");
    printf("\n; Perform a RET from the [EBP] stack");
    printf("\nRETfromEBP:");
    printf("\n\tPUSH DWORD [EBP]");
    printf("\n\tSUB  EBP, 4");
    printf("\n\tRET");
}

void genCode() {
    genStartupCode();

    int i = 1;
    while (i <= here) {
        int op = opcodes[i];
        int a1 = arg1[i];
        int a2 = arg2[i];
        char *SW = "\n\tXCHG ESP, EDI";
        // printf("\n; %3d: %-3d %-3d %-5d\n\t", i, op, a1, a2);
        if (op == VARADDR) { printf("\n\tPUSH EAX\n\tLEA EAX, [%s] ; %s", asmName(a1), varName(a1)); }
        else if (op == LIT)     { printf("\n\tPUSH EAX\n\tMOV EAX,%d", a1); }
        else if (op == LOADSTR) { printf("\n\tPUSH EAX\n\tLEA EAX, [%s]", strings[a1].name); }
        else if (op == STORE)   { printf("\n\tPOP ECX\n\tMOV [EAX], ECX\n\tPOP EAX"); }
        else if (op == FETCH)   { printf("\n\tMOV EAX, [EAX]"); }
        else if (op == PLEQ)    { printf("\n\tPOP EAX\n\tADD [%s], EAX", asmName(a1)); }
        else if (op == DECTOS)  { printf("\n\tDEC EAX"); }
        else if (op == INCTOS)  { printf("\n\tINC EAX"); }
        else if (op == ADD)     { printf("\n\tPOP EBX\n\tADD EAX, EBX"); }
        else if (op == SUB)     { printf("\n\tPOP EBX\n\tXCHG EAX, EBX\n\tSUB EAX, EBX"); }
        else if (op == MULT)    { printf("\n\tPOP EBX\n\tIMUL EAX, EBX"); }
        else if (op == DIVIDE)  { printf("\n\tPOP EBX\n\tXCHG EAX, EBX\n\tCDQ\n\tIDIV EBX"); }
        else if (op == LT)      { printf("\n\tPOP EBX\n\tCMP EBX, EAX\n\tMOV EAX, 0\n\tJGE @F\n\tDEC EAX\n@@:"); }
        else if (op == GT)      { printf("\n\tPOP EBX\n\tCMP EBX, EAX\n\tMOV EAX, 0\n\tJLE @F\n\tDEC EAX\n@@:"); }
        else if (op == EQ)      { printf("\n\tPOP EBX\n\tCMP EBX, EAX\n\tMOV EAX, 0\n\tJNZ @F\n\tDEC EAX\n@@:"); }
        else if (op == NEQ)     { printf("\n\tCMP_NEQ"); }
        else if (op == DEF)     { printf("\n\n%s: ; %s\n\tCALL RETtoEBP", asmName(a1), varName(a1)); }
        else if (op == CALL)    { printf("\n\tCALL %s ; %s (%d)", asmName(a1), varName(a1), a1); }
        else if (op == PARAM)   { printf("\n\t; PARAM"); }
        else if (op == RETURN)  { printf("\n\tJMP RETfromEBP"); }
        else if (op == TARGET)  { printf("\n%s:", varName(a1)); }
        else if (op == JMP)     { printf("\n\tJMP %s", varName(a1)); }
        else if (op == JMPZ)    { printf("\n\tTEST EAX, EAX\n\tJZ %s", varName(a1)); }
        else if (op == JMPNZ)   { printf("\n\tTEST EAX, EAX\n\tJNZ %s", varName(a1)); }
        i++;
    }
}

//---------------------------------------------------------------------------
// Parser / code generator.
void winLin(int seg) {
#ifdef _WIN32
    // Windows (32-bit)
    if (seg == 'C') {
        char *pv = asmName(findVar("pv", 'I'));
        printf("format PE console");
        printf("\ninclude 'win32ax.inc'\n");
        printf("\n; ======================================= ");
        printf("\nsection '.code' code readable executable");
        printf("\n;=======================================*/");
        printf("\nstart:\n\tCALL init");
        printf("\n\tCALL %s\n", asmName(findVar("main", 'F')));
        printf("\n;================== library ==================");
        printf("\n%s:\n\tPUSH 0\n\tCALL [ExitProcess]\n", asmName(findVar("bye", 'F')));
        P("\n;=============================================");
        printf("\n%s: ; puts", asmName(findVar("puts", 'F')));
        printf("\n\tCALL RETtoEBP\n\tMOV [%s], EAX\n\tPOP EAX", pv);
        printf("\n\tcinvoke printf, \"%s\", [%s]", "%s", pv);
        printf("\n\tJMP RETfromEBP\n");
        
        printf("\n%s: ; emit", asmName(findVar("emit", 'F')));
        printf("\n\tCALL RETtoEBP\n\tMOV [%s], EAX\n\tPOP EAX", pv);
        printf("\n\tcinvoke printf, \"%s\", [%s]", "%c", pv);
        printf("\n\tJMP RETfromEBP\n");
        
        printf("\n%s: ; .d", asmName(findVar(".d", 'F')));
        printf("\n\tCALL RETtoEBP\n\tMOV [%s], EAX\n\tPOP EAX", pv);
        printf("\n\tcinvoke printf, \"%s\", [%s]", "%d", pv);
        printf("\n\tJMP RETfromEBP\n");
    }
    else if (seg == 'D') {
        printf("\n\n;================== data =====================");
        printf("\nsection '.data' data readable writeable");
        printf("\n;=============================================");
    }
    else if (seg == 'I') {
        printf("\n;====================================");
        printf("\nsection '.idata' import data readable");
        printf("\n; ====================================");
        printf("\nlibrary msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'");
        printf("\nimport msvcrt, printf,'printf', getch,'_getch'");
        printf("\nimport kernel32, ExitProcess,'ExitProcess'\n");
    }
#else
    // Linux (32-bit)
    if (seg == 'C') {
        char *pv = asmName(findVar("pv", 'I'));
        printf("format ELF executable");
        printf("\n;================== code =====================");
        printf("\nsegment readable executable");
        printf("\n;================== library ==================");
        printf("\nstart:\n\tCALL init");
        printf("\n\tCALL %s\n", asmName(findVar("main", 'F')));
        printf("\n%s:", asmName(findVar("bye", 'F')));
        printf("\n\tMOV EAX, 1\n\tXOR EBX, EBX\n\tINT 0x80\n");

        printf("\n\n%s: ; puts", asmName(findVar("puts", 'F')));
        printf("\n\tCALL RETtoEBP");
        printf("\n\tMOV [%s], EAX", pv);
        printf("\n\tPOP EAX\n\tJMP RETfromEBP");
        
        printf("\n\n%s:\n\tCALL RETtoEBP", asmName(findVar(".d", 'F')));
        printf("\n\tMOV [%s], EAX", pv);
        printf("\n\tPOP EAX\n\tJMP RETfromEBP");
        
        printf("\n%s: ; emit", asmName(findVar("emit", 'F')));
        printf("\n\tCALL RETtoEBP");
        printf("\n\n\tMOV [%s], EAX", pv);
        printf("\n\tMOV EAX, 4");
        printf("\n\tMOV EBX, 0");
        printf("\n\tLEA ECX, [%s]", pv);
        printf("\n\tMOV EDX, 1");
        printf("\n\tINT 0x80\n\tJMP RETfromEBP\n");
        printf("\n;=============================================");
    }
    else if (seg == 'D') {
        printf("\n;================== data =====================");
        printf("\nsegment readable writeable");
        printf("\n;=============================================");
    }
    else if (seg == 'I') {
        // printf("\n;================== data =====================");
        // printf("\nsegment readable writeable");
        // printf("\n;=============================================");
    }
#endif
    if (seg == 'S') {
        int s = addFunction("bye");
        s = addFunction("puts");
        s = addFunction("emit");
        s = addFunction(".d");
        s = addVar("pv", 'I');
    }
}

void ifStmt() {
    int i = genTargetSymbol();
    push(i);
    gen1(JMPZ, i);
}

void elseStmt() {
    gen(JMP);
    gen(JMPZ);
}

void thenStmt() {
    int i = pop();
    gen1(TARGET, i);
}

void beginStmt() {
    int i = genTargetSymbol();
    push(i);
    gen1(TARGET, i);
}

void whileStmt() {
    int i = pop();
    gen1(JMPNZ, i);
}

void untilStmt() {
    int i = pop();
    gen1(JMPZ, i);
}

void againStmt() {
    int i = pop();
    gen1(JMP, i);
}

char tmpStr[256];
void stringStmt() {
    int i = 0;
    next_ch();
    while (ch != '"') {
        if (ch == EOF) { syntax_error(); }
        tmpStr[i++] = ch;
        next_ch();
    }
    tmpStr[i] = 0;
    next_ch();
    i = addString(tmpStr);
    gen1(LOADSTR, i);
}

void statement() {
    if (is_num) { gen1(LIT, int_val); return; }
    int i = findVar(token, 'I');
    if (i) { gen1(VARADDR, i); return; }
    i = findVar(token, 'F');
    if (i) { gen1(CALL, i); return; }

    if (accept("@"))    { gen(FETCH); }
    else if (accept("!"))     { gen(STORE); }
    else if (accept("1+"))    { gen(INCTOS); }
    else if (accept("1-"))    { gen(DECTOS); }
    else if (accept("if"))    { ifStmt(); }
    else if (accept("else"))  { thenStmt(); }
    else if (accept("then"))  { thenStmt(); }
    else if (accept("begin")) { beginStmt(); }
    else if (accept("while")) { whileStmt(); }
    else if (accept("until")) { untilStmt(); }
    else if (accept("again")) { againStmt(); }
    else if (accept("exit"))  { gen(RETURN); }
    else if (accept("drop"))  { gen(DROP); }
    else if (accept(";"))     { gen(RETURN); }
    else if (accept("+"))     { gen(ADD); }
    else if (accept("-"))     { gen(SUB); }
    else if (accept("*"))     { gen(MULT); }
    else if (accept("/"))     { gen(DIVIDE); }
    else if (accept("<"))     { gen(LT); }
    else if (accept(">"))     { gen(EQ); }
    else if (accept("="))     { gen(GT); }
    else if (accept("\""))    { stringStmt(); }
    else if (accept(""))      { return; }
    else { syntax_error(); }
}

void funcDef() {
    next_token();
    int s = addVar(token, 'F');
    gen1(DEF, s);
    while (1) {
        next_token();
        statement();
        if (accept(";")) { return; }
    }
}

void parseVar() {
    next_token();
    addVar(token, 'I');
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
    winLin('S');
    while (ch != EOF) { next_token(); parseDef(); }
    if (input_fp) { fclose(input_fp); }
    optimizeIRL();
    winLin('C');
    genCode();
    winLin('D');
    dumpSymbols();
    winLin('I');
    return 0;
}
