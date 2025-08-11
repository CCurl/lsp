/* Chris Curl, MIT license. */
/* Based on work by Marc Feeley (2001), MIT license. */
/* Please see the README.md for details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYMBOLS_SZ   500
#define CODE_SZ     2500
#define NODES_SZ   10000

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
    , TOK_SET, TOK_NUM, TOK_ID, TOK_FUNC
    , TOK_OR, TOK_AND, TOK_XOR, TOK_LOR, TOK_LAND, TOK_LNOT
    , TOK_SEMI, EOI
};

// Syntax tree node types
enum {
    ND_VAR, ND_CST, ND_ADD, ND_SUB, ND_MUL, ND_DIV
    , ND_LT, ND_EQ, ND_GT, ND_NEQ, ND_SET
    , ND_AND, ND_OR, ND_XOR, ND_LAND, ND_LOR, ND_LNOT
    , ND_FUNC_CALL, ND_FUNC_DEF
    , ND_IF1, ND_IF2, ND_WHILE, ND_DO, ND_EMPTY, ND_SEQ, ND_PROG
    , ND_RET
};

// VM opcodes
enum {
    NOP, IFETCH, ISTORE, ILIT, IDROP
    , ILT, IGT, IEQ, INEQ, ILAND, ILOR, ILNOT
    , IADD, ISUB, IMUL, IDIV
    , IAND, IOR, IXOR
    , JZ, JNZ, JMP, ICALL, IRET, HALT
    , IMOV
};

byte vm[CODE_SZ];
extern int findSymbolVal(char type, long val);
extern void dumpSymbols(int details, FILE *toFP);

 /*---------------------------------------------------------------------------*/
 /* Lexer. */

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens
char *words[] = { "do", "else", "if"
    , "while", "void", "int", "byte"
    , "return", NULL};

int ch = ' ', tok, int_val;
char id_name[64];
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
    case '!': next_ch(); // tok = TOK_LNOT;
        if (ch == '=') { tok = TOK_NEQ; next_ch(); }
        else { syntax_error(); }
        break;
    case '|': next_ch(); tok = TOK_OR;
        if (ch == '|') { tok = TOK_LOR; next_ch(); }
        break;
    case '&': next_ch(); tok = TOK_AND;
        if (ch == '&') { tok = TOK_LAND; next_ch(); }
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
    return i;
}

void dumpSymbols(int details, FILE *toFP) {
    FILE *fp = toFP ? toFP : stdout;
    fprintf(fp, "symbols: %d entries, %d used\n", SYMBOLS_SZ, numSymbols);
    fprintf(fp, "num type size val       name\n");
    fprintf(fp, "--- ---- ---- --------- -----------------\n");
    if (details) {
        for (int i = 0; i < numSymbols; i++) {
            SYM_T *x = &symbols[i];
            fprintf(fp, "%-3d %-4d %-4d $%-8lX %s\n", 
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
        x = new_node(ND_CST); // CONSTANT
        x->val = int_val;
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
    else if (tok == TOK_LBRA) { /* "{" <statement> "}" */
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
int memBase, here, vmHere, addrSz;

void s1(int a, int v) { vm[a] = (v & 255); }
void s2(int a, int v) { s1(a,v); s1(a+1,v>>8); }
void s4(int a, int v) { s2(a,v); s1(a+2,v>>16); }

void g(int c) { s1(here, c); here=here+1; vmHere=vmHere+1; }
void g2(int n) { g(n); g(n>>8); }
void g4(int n) { g2(n); g2(n>>16); }
void gAddr(int a) { g2(a); if (addrSz==4) { g2(a>>16); } }

int hole() { gAddr(0); return here-addrSz; }
void fix(int a, int v) {  s2(a, v); if (addrSz==4) { s2(a+2, v>>16); } }

// ----------------------------------------------------------
// change these to generate code for a different architecture
void gInit() {
    here = 0;
    memBase = 0; // 0x08048000; // Linux 23-bit code start (134512640)
    addrSz = 2;
}

void gMov1(int arg1) { g(0x89); g(arg1); }
void gFetch(int v) { g(IFETCH); gAddr(v); }
void gStore(int v) { g(ISTORE); gAddr(v); }
void gLit(int v) { g(ILIT); g4(v); }
void gCall(int v) { g(ICALL); gAddr(v); }
void gReturn() { g(IRET); }
void gAdd() { g(IADD); }
void gMul() { g(IMUL); }
void gSub() { g(ISUB); }
void gDiv() { g(IDIV); }
void gLT() { g(ILT); }
void gGT() { g(IGT); }
void gEQ() { g(IEQ); }
void gNEQ() { g(INEQ); }
void gLAnd() { g(ILAND); }
void gLOr() { g(ILOR); }
void gLNot() { g(ILNOT); }
void gAnd() { g(IAND); }
void gOr() { g(IOR); }
void gXor() { g(IXOR); }
void gJmp()   { g(JMP); }
void gJmpZ()  { g(JZ); }
void gJmpNZ() { g(JNZ); }
void gBye() { g(HALT); }
// ----------------------------------------------------------

void c(node *x) {
    int p1, p2;
    switch (x->kind) {
        case ND_VAR:  gFetch(x->val); break;
        case ND_CST:  gLit(x->val); break;
        case ND_ADD:  c(x->o1); c(x->o2); gAdd();  break;
        case ND_MUL:  c(x->o1); c(x->o2); gMul();  break;
        case ND_SUB:  c(x->o1); c(x->o2); gSub();  break;
        case ND_DIV:  c(x->o1); c(x->o2); gDiv();  break;
        case ND_LT:   c(x->o1); c(x->o2); gLT();   break;
        case ND_GT:   c(x->o1); c(x->o2); gGT();   break;
        case ND_EQ:   c(x->o1); c(x->o2); gEQ();   break;
        case ND_NEQ:  c(x->o1); c(x->o2); gNEQ();  break;
        case ND_LAND: c(x->o1); c(x->o2); gLAnd(); break;
        case ND_LOR:  c(x->o1); c(x->o2); gLOr();  break;
        case ND_LNOT: c(x->o1); c(x->o2); gLNot(); break;
        case ND_AND:  c(x->o1); c(x->o2); gAnd();  break;
        case ND_OR:   c(x->o1); c(x->o2); gOr();   break;
        case ND_XOR:  c(x->o1); c(x->o2); gXor();  break;
        case ND_SET:  c(x->o2); gStore(x->o1->val); break;
        case ND_IF1:  c(x->o1); gJmpZ(); p1 = hole(); c(x->o2); fix(p1, vmHere); break;
        case ND_IF2:  c(x->o1); gJmpZ(); p1 = hole(); c(x->o2);
            gJmp(); p2 = hole(); fix(p1, vmHere);
            c(x->o3); fix(p2, vmHere); break;
        case ND_WHILE: p1 = vmHere; c(x->o1); gJmpZ(); p2 = hole(); c(x->o2);
            gJmp(); gAddr(p1); fix(p2, vmHere); break;
        case ND_DO: p1 = vmHere; c(x->o1); c(x->o2); gJmpNZ(); gAddr(p1); break;
        case ND_EMPTY: break;
        case ND_SEQ: c(x->o1); c(x->o2); break;
        case ND_FUNC_CALL: if (x->val == 0) { error("undefined function!"); }
            gCall(x->val); break;
        case ND_FUNC_DEF: c(x->o1); break;
        case ND_PROG: c(x->o1); gBye();  break;
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
            symbols[sym].val = vmHere;
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
    gInit();
    gJmp();
    gAddr(0);
    defs(NULL);
    if (input_fp != stdin) { fclose(input_fp); }
    printf("%d code bytes (%d nodes)\n", here, num_nodes);

    int mainSym = findSymbol("main", TOK_FUNC);
    if (mainSym < 0) { printf("no main() function!"); }
    fix(1, symbols[mainSym].val);
    FILE *fp = fopen("tc.out", "wb");
    if (fp) { fwrite(vm, 1, here, fp); fclose(fp); }

    fp = fopen("tc.sym", "wt");
    dumpSymbols(1, fp);
    fclose(fp);

    return 0;
}
