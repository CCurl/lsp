/* file: "tc.c" */
/* originally from http://www.iro.umontreal.ca/~felipe/IFT2030-Automne2002/Complements/tinyc.c */
/* Copyright (C) 2001 by Marc Feeley, All Rights Reserved. */
/* Modified by Chris Curl */

#include "tc.h"

/*
 * This is a compiler for the Tiny-C language.  Tiny-C is a
 * considerably stripped down version of C and it is meant as a
 * pedagogical tool for learning about compilers.  The integer global
 * variables "a" to "z" are predefined and initialized to zero, and it
 * is not possible to declare new variables.  The compiler reads the
 * program from standard input and prints out the value of the
 * variables that are not zero.  The grammar of Tiny-C in EBNF is:
 * 
 * <program> ::= <defs>
 * <defs> ::= <def> | <def> <def>
 * <def> ::= <func_def> | <var_def>
 * <func_def> ::= "void" <id> "(" ")" "{" <statement> "}" |
 * <var_def> ::= "int" <id> ";"
 * <statement> ::= "if" <paren_expr> <statement> |
 *                 "if" <paren_expr> <statement> "else" <statement> |
 *                 "while" <paren_expr> <statement> |
 *                 "do" <statement> "while" <paren_expr> ";" |
 *                 "{" { <statement> } "}" |
 *                 <func> ";" |
 *                 <expr> ";" |
 *                 ";"
 * <paren_expr> ::= "(" <expr> ")"
 * <expr> ::= <test> | <id> "=" <expr>
 * <test> ::= <math> | <math> "<" <math> | <math> ">" <math> | <math> "==" <math>
 * <math> ::= <term> | <math> <math_op> <term>
 * <math_op> ::= "+" | "-" | "*" | "/"
 * <term> ::= <id> | <int> | <paren_expr>
 * <id> ::= "a" | "b" | "c" | "d" | ... | "z" -- FOR NOW
 * <id> ::= [A-Z|a-z][A-Z|a-z|0-9|_]* -- NEW
 * <int> ::= <an_unsigned_decimal_integer>
 * <func> ::= <id> "(" ")"
 *
 * The compiler does a minimal amount of error checking to help
 * highlight the structure of the compiler.
 */

 /*---------------------------------------------------------------------------*/
 /* Lexer. */

#define BTWI(n,l,h) ((l<=n)&&(n<=h))

// Tokens
enum {
    DO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK, VOID_TOK, INT_TOK, //  0->5
    LBRA, RBRA, LPAR, RPAR,                                 //  6->9
    PLUS, MINUS, STAR, SLASH, LESS, GRT, SEMI, EQUAL,       // 10->17
    INT, ID, EOI, FUNC_TOK, EQU                             // 18->22
};

// NOTE: these have to be in sync with the first <x> entries in the Symbols list above
char *words[] = { "do", "else", "if", "while", "void", "int", NULL };

int ch = ' ', tok, int_val;
char id_name[64];
FILE *input_fp = NULL;

void message(char *msg, int cr) { fprintf(stdout, "%s%s", msg, cr ? "\n" : " "); }
void warn(char *msg)  { message("WARN:",0); message(msg, 1); }
void error(char *err) { message("ERROR:",0); message(err, 1); exit(1); }
void syntax_error() { error("syntax error"); }

int isAlpha(int ch) { return BTWI(ch, 'A', 'Z') || BTWI(ch, 'a', 'z') || (ch == '_'); }
int isNum(int ch) { return BTWI(ch, '0', '9'); }
int isAlphaNum(int ch) { return isAlpha(ch) || isNum(ch); }

void next_ch() {
    if (input_fp) { ch = fgetc(input_fp); }
    else { ch = getchar(); }
    // if (BTWI(ch,32,126)) { printf("%c", ch); } else { printf("(%d)", ch); }
}

void next_token() {
    while (BTWI(ch,1,32)) { next_ch(); }
    switch (ch) {
    case EOF: tok = EOI; break;
    case '{': next_ch(); tok = LBRA;  break;
    case '}': next_ch(); tok = RBRA;  break;
    case '(': next_ch(); tok = LPAR;  break;
    case ')': next_ch(); tok = RPAR;  break;
    case '+': next_ch(); tok = PLUS;  break;
    case '-': next_ch(); tok = MINUS; break;
    case '*': next_ch(); tok = STAR;  break;
    case '/': next_ch(); tok = SLASH; break;
    case '<': next_ch(); tok = LESS;  break;
    case '>': next_ch(); tok = GRT;   break;
    case ';': next_ch(); tok = SEMI;  break;
    case '=': tok = EQUAL; next_ch();
        if (ch == '=') { tok = EQU; next_ch(); }
        break;
    default:
        if (isNum(ch)) {
            int_val = 0; /* missing overflow check */
            while (isNum(ch)) { int_val = int_val  *10 + (ch - '0'); next_ch(); }
            tok = INT;
        }
        else if (isAlpha(ch)) {
            int i = 0; /* missing overflow check */
            while (isAlphaNum(ch)) { id_name[i++] = ch; next_ch(); }
            id_name[i] = '\0';
            tok = 0;
            while (words[tok] != NULL && strcmp(words[tok], id_name) != 0) { tok++; }
            if (words[tok] == NULL) {
                tok = ID;
                if (ch == '(') {
                    next_ch();
                    if (ch == ')') { tok = FUNC_TOK; next_ch(); }
                    else { syntax_error(); }
                }
            }
        }
        else { message("-ch-", 0); syntax_error(); }
        break;
    }
}

/*---------------------------------------------------------------------------*/
/* Symbols */

#define SYMBOLS_SZ 500
typedef struct { char type, *name; int32_t val; } SYM_T;
SYM_T symbols[SYMBOLS_SZ];
int numSymbols = 0;

int genSymbol(char *name, char type) {
    for (int i = 0; i < numSymbols; i++) {
        if (strcmp(symbols[i].name, name) == 0) { return i; }
    }
    SYM_T *x = &symbols[numSymbols];
    x->name = hAlloc(strlen(name) + 1);
    x->val = 0;
    x->type = type;
    strcpy(x->name, name);
    return numSymbols++;
}

void dumpSymbols(int details, FILE *toFP) {
    fprintf(toFP ? toFP : stdout, "symbols: %d, %d used\n", SYMBOLS_SZ, numSymbols);
    if (details) {
        for (int i = 0; i < numSymbols; i++) {
            SYM_T *x = &symbols[i];
            fprintf(toFP ? toFP : stdout, "%3d - type: %2d, val: %d  %s\n", 
                i, x->type, x->val, x->name);
        }
    }
}

/*---------------------------------------------------------------------------*/
/* Parser. */

enum {
    VAR, CST, ADD, SUB, MUL, DIV, LT, GT, SET, FUNC_CALL,
    IF1, IF2, WHILE, DO, EMPTY, SEQ, EXPR, PROG
};

#define MAX_NODES 1000
struct node_s { int kind; struct node_s *o1,  *o2,  *o3; int val, sval; };
typedef struct node_s node;
int num_nodes = 0;
node nodes[MAX_NODES];

node *new_node(int k) {
    if (MAX_NODES <= num_nodes) { error("no nodes!"); }
    node *x = &nodes[num_nodes++];
    x->kind = k;
    return x;
}

node *gen(int k, node *o1, node *o2) {
    node *x = new_node(k);
    x->o1 = o1; x->o2 = o2;
    return x;
}

void expect_token(int exp) {
    if (tok != exp) {
        printf("-expected symbol[%d],not[%d]-", exp, tok);
        syntax_error();
    }
    next_token();
}

node *paren_expr(); /* forward declaration */


/* <term> ::= <id> | <int> | <paren_expr> */
node *term() {
    node *x;
    if (tok == ID) {
        x = new_node(VAR);
        x->sval = genSymbol(id_name, VAR);
        x->val = id_name[0] - 'a'; // Update this for longer names
        next_token();
    }
    else if (tok == INT) {
        x = new_node(CST); // CONSTANT
        x->val = int_val;
        next_token();
    }
    else x = paren_expr();
    return x;
}

int mathop() {
    if (tok == PLUS) { return ADD; }
    else if (tok == MINUS) { return SUB; }
    else if (tok == STAR) { return MUL; }
    else if (tok == SLASH) { return DIV; }
    return 0;
}

/* <math> ::= <term> | <math> <math_op> <term> */
/* <math_op> ::= "+" | "-" | "*" | "/" */
node *sum() {
    node *x = term();
    while (mathop()) {
        x = gen(mathop(), x, 0);
        next_token();
        x->o2 = term();
    }
    return x;
}

/* <test> ::= <math> | <math> "<" <math> | <math> ">" <math> */
node *test() {
    node *x = sum();
    if (tok == LESS) { next_token(); return gen(LT, x, sum()); }
    if (tok == GRT) { next_token(); return gen(GT, x, sum()); }
    if (tok == EQU) { next_token(); return gen(EQU, x, sum()); }
    return x;
}

/* <expr> ::= <test> | <id> "=" <expr> */
node *expr() {
    node *x;
    if (tok != ID) { return test(); }
    x = test();
    if ((x->kind == VAR) && (tok == EQUAL)) {
        next_token();
        return gen(SET, x, expr());
    }
    return x;
}

/* <paren_expr> ::= "(" <expr> ")" */
node *paren_expr() {
    node *x;
    expect_token(LPAR);
    x = expr();
    expect_token(RPAR);
    return x;
}

node *statement() {
    node *x = NULL;
    if (tok == IF_TOK) { /* "if" <paren_expr> <statement> */
        x = new_node(IF1);
        next_token();
        x->o1 = paren_expr();
        x->o2 = statement();
        if (tok == ELSE_TOK) { /* ... "else" <statement> */
            x->kind = IF2;
            next_token();
            x->o3 = statement();
        }
    }
    else if (tok == WHILE_TOK) { /* "while" <paren_expr> <statement> */
        x = new_node(WHILE);
        next_token();
        x->o1 = paren_expr();
        x->o2 = statement();
    }
    else if (tok == FUNC_TOK) { /* <id> "();" */
        x = new_node(FUNC_CALL);
        x->sval = genSymbol(id_name, FUNC_TOK);
        printf("-call %s()-", id_name);
        // TODO: call the function
        x->val = 12345;
        next_token();
        expect_token(SEMI);
    }
    else if (tok == DO_TOK) { /* "do" <statement> "while" <paren_expr> ";" */
        x = new_node(DO);
        next_token();
        x->o1 = statement();
        expect_token(WHILE_TOK);
        x->o2 = paren_expr();
        expect_token(SEMI);
    }
    else if (tok == SEMI) { /* ";" */
        x = new_node(EMPTY);
        next_token();
    }
    else if (tok == LBRA) { /* "{" <statement> "}" */
        x = new_node(EMPTY);
        next_token();
        while (tok != RBRA) {
            x = gen(SEQ, x, 0);
            x->o2 = statement();
        }
        next_token();
    }
    else if (tok == EQU) { /* <id> = <expr> ; */
        
    }
    else { /* <expr> ";" */
        x = gen(EXPR, expr(), NULL);
        expect_token(SEMI);
    }
    return x;
}

/* <program> ::= <statement> */
node *defs(node *st) {
    node *x = st;
    next_token();
    while (1) {
        if (tok == EOI) { break; }
        if (tok == LBRA) { break; }
        if (tok == VOID_TOK) {
            next_token(); expect_token(FUNC_TOK);
            printf("-def %s()-", id_name);
            genSymbol(id_name, FUNC_TOK);
            // TODO: Add the function
            expect_token(LBRA);
            x = new_node(EMPTY);
            while (tok != RBRA) {
                x = gen(SEQ, x, NULL);
                x->o2 = statement();
            }
            next_token();
            continue;
        }
        if (tok == INT_TOK) {
            next_token(); expect_token(ID);
            genSymbol(id_name, VAR);
            // printf("-VAR %s-", id_name);
            expect_token(SEMI);
            continue;
        }
        message("-def?-", 0); syntax_error();
    }
    return st;
}

/* <program> ::= <statement> */
node *program() {
    node *prog = gen(PROG, NULL, NULL);
    node *code = defs(prog);
    if (tok != EOI) {
        if (code->kind != PROG) { code = gen(SEQ, code, NULL); }
        code->o1 = statement();
    }
    return prog;
}

/*---------------------------------------------------------------------------*/
/* Code generator. */

code *here = &vm[0];

void g(code c) { *here++ = c; } /* missing overflow check */
void g4(int n) {
    g(n & 0xff); n = (n >> 8);
    g(n & 0xff); n = (n >> 8);
    g(n & 0xff); n = (n >> 8);
    g(n & 0xff);
}
void g2(int n) {
    g(n & 0xff); n = (n >> 8);
    g(n & 0xff);
}
code *hole() { return here++; }
void fix(code *src, code *dst) { *src = dst - src; } /* missing overflow check */

void c(node *x) {
    code *p1, *p2;
    switch (x->kind) {
    case VAR: g(IFETCH); g2(x->val); break;
    case CST: if (BTWI(x->val, 0, 127)) { g(IP1); g(x->val); }
            else if (BTWI(x->val, 128, 32767)) { g(IP2); g2(x->val); }
            else { g(IP4); g4(x->val); }
            break;
    case ADD: c(x->o1);  c(x->o2); g(IADD); break;
    case MUL: c(x->o1);  c(x->o2); g(IMUL); break;
    case SUB: c(x->o1);  c(x->o2); g(ISUB); break;
    case DIV: c(x->o1);  c(x->o2); g(IDIV); break;
    case LT: c(x->o1);  c(x->o2); g(ILT); break;
    case GT: c(x->o1);  c(x->o2); g(IGT); break;
    case EQU: c(x->o1);  c(x->o2); g(IEQ); break;
    case SET: c(x->o2);  g(ISTORE); g2(x->o1->val); break;
    case IF1: c(x->o1);  g(JZ); p1 = hole(); c(x->o2); fix(p1, here); break;
    case IF2: c(x->o1);  g(JZ); p1 = hole(); c(x->o2); g(JMP); p2 = hole();
        fix(p1, here); c(x->o3); fix(p2, here); break;
    case WHILE: p1 = here; c(x->o1); g(JZ); p2 = hole(); c(x->o2);
        g(JMP); fix(hole(), p1); fix(p2, here); break;
    case DO: p1 = here; c(x->o1); c(x->o2); g(JNZ); fix(hole(), p1); break;
    case EMPTY: break;
    case SEQ: c(x->o1); c(x->o2); break;
    case EXPR: c(x->o1); g(IDROP); break;
    case PROG: c(x->o1); g(HALT);  break;
    case FUNC_CALL: g(ICALL); g2(x->val); break;
    }
}

/*---------------------------------------------------------------------------*/
/* Main program. */

int main(int argc, char *argv[]) {
    if (argc > 1) { input_fp = fopen(argv[1], "rt"); }
    if (argc > 1) { input_fp = fopen(argv[1], "rt"); }
    else { input_fp = fopen("test.tc", "rt"); }
    c(program());

    printf("\n(%d nodes, %d code bytes)\n", num_nodes, (int)(here - &vm[0]));
    for (int i = 0; i < 26; i++) { globals[i] = 0; }
    initVM(vm);
    runVM(vm);
    for (int i = 0; i < 26; i++) {
        if (globals[i] != 0) printf("%c = %ld\n", 'a' + i, globals[i]);
    }
    dis(here);
    printf("\n");
    return 0;
}
