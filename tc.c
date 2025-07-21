/* Chris Curl, MIT license. */
/* Based on work by Marc Feeley (2001), MIT license. */
/* Please see the README.md for details. */

#include "tc.h"

 /*---------------------------------------------------------------------------*/
 /* Lexer. */

// NOTE: these have to be in sync with the first <x> entries in the 
// list of tokens defined in tc.h
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
        for (int i=1; i<cur_off; i++) { fprintf(stdout, " "); }
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
    case '{': next_ch(); tok = LBRA;  break;
    case '}': next_ch(); tok = RBRA;  break;
    case '(': next_ch(); tok = LPAR;  break;
    case ')': next_ch(); tok = RPAR;  break;
    case '[': next_ch(); tok = LARR;  break;
    case ']': next_ch(); tok = RARR;  break;
    case '+': next_ch(); tok = PLUS;  break;
    case '-': next_ch(); tok = MINUS; break;
    case '*': next_ch(); tok = STAR;  break;
    case '/': next_ch(); tok = SLASH;
        if (ch == '/') { // Line comment?
            while ((ch != 10) && (ch != EOF)) { next_ch(); }
            goto again;
        }
        break;
    case '<': next_ch(); tok = LESS;  break;
    case '>': next_ch(); tok = GRT;   break;
    case ';': next_ch(); tok = SEMI;  break;
    case '=': next_ch(); tok = EQUAL;
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
            while ((words[tok] != NULL) && (strcmp(words[tok], id_name) != 0)) { tok++; }
            if (words[tok] == NULL) {
                tok = ID;
                if (ch == '(') {
                    next_ch();
                    if (ch == ')') { tok = FUNC_TOK; next_ch(); }
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
        // printf("-expected token [%d], not[%d]-", exp, tok);
        syntax_error();
    }
    next_token();
}

/*---------------------------------------------------------------------------*/
/* Symbols */

SYM_T symbols[SYMBOLS_SZ];
int numSymbols = 0;

int findSymbolVal(char type, long val) {
    for (int i = 0; i < numSymbols; i++) {
        SYM_T *x = &symbols[i];
        if ((x->type == type) && (x->val == val)) { return i; }
    }
    error("symbol not defined");
    return 0;
}

int genSymbol(char *name, char type) {
    SYM_T *x;
    for (int i = 0; i < numSymbols; i++) {
        x = &symbols[i];
        if (strcmp(x->name, name) == 0) {
            if (x->type == type) { return i; }
            error("name already defined with different type.");
        }
    }
    x = &symbols[numSymbols];
    x->name = hAlloc(strlen(name) + 1);
    x->val = 0;
    x->type = type;
    x->sz = 4;
    strcpy(x->name, name);
    return numSymbols++;
}

void dumpSymbols(int details, FILE *toFP) {
    FILE *fp = toFP ? toFP : stdout;
    fprintf(fp, "symbols: %d entries, %d used\n", SYMBOLS_SZ, numSymbols);
    fprintf(fp, "num type size val      name\n");
    fprintf(fp, "----------------------------------------\n");
    if (details) {
        for (int i = 0; i < numSymbols; i++) {
            SYM_T *x = &symbols[i];
            fprintf(fp, "%-3d %-4d %-4d %-8ld %s\n", 
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
    x->o1 = o1; x->o2 = o2;
    return x;
}

node *paren_expr(); /* forward declaration */


/* <term> ::= <id> | <int> | <paren_expr> */
node *term() {
    node *x;
    if (tok == ID) {
        x = new_node(VAR);
        x->sval = genSymbol(id_name, VAR);
        x->val = x->sval;
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
    if (tok == EQU) { next_token(); return gen(EQ, x, sum()); }
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
        x->val = symbols[x->sval].val;
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
    else if (tok == RET_TOK) { /* "return" ";"*/
        x = new_node(RET);
        next_token();
        expect_token(SEMI);
    }
    else if (tok == SEMI) { /* ";" */
        x = new_node(EMPTY);
        next_token();
    }
    else if (tok == LBRA) { /* "{" <statement> "}" */
        int seqNo = 1;
        x = new_node(EMPTY);
        next_token();
        while (tok != RBRA) {
            x = gen(SEQ, x, 0);
            x->val = seqNo;
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


/*---------------------------------------------------------------------------*/
/* Code generator. */

void g(int c) { vm[here++] = (c & 0xff); }
void g2(int n) { g(n); g(n>>8); }
void g4(int n) { g(n); g(n>>8); g(n>>16); g(n>>24); }

int hole(int inst) {
    g(inst);
    int h = here;
    g2(0);
    return h;
}

void fix(int src, int dst) {
    vm[src+0] = (dst & 0xff); dst = (dst >> 8);
    vm[src+1] = (dst & 0xff);
}

void c(node *x) {
    int p1, p2;
    switch (x->kind) {
        case VAR: g(IFETCH); g2(x->val); break;
        case CST: if (BTWI(x->val, 0, 127)) { g(IP1); g(x->val); }
                else if (BTWI(x->val, 128, 32767)) { g(IP2); g2(x->val); }
                else { g(IP4); g4(x->val); }
                break;
        case ADD: c(x->o1); c(x->o2); g(IADD); break;
        case MUL: c(x->o1); c(x->o2); g(IMUL); break;
        case SUB: c(x->o1); c(x->o2); g(ISUB); break;
        case DIV: c(x->o1); c(x->o2); g(IDIV); break;
        case LT:  c(x->o1); c(x->o2); g(ILT);  break;
        case GT:  c(x->o1); c(x->o2); g(IGT);  break;
        case EQ:  c(x->o1); c(x->o2); g(IEQ);  break;
        case SET: c(x->o2); g(ISTORE); g2(x->o1->val); break;
        case IF1: c(x->o1); p1 = hole(JZ); c(x->o2); fix(p1, here); break;
        case IF2: c(x->o1); p1 = hole(JZ); c(x->o2);
            p2 = hole(JMP); fix(p1, here);
            c(x->o3); fix(p2, here); break;
        case WHILE: p1 = here; c(x->o1); p2 = hole(JZ); c(x->o2);
            fix(hole(JMP), p1); fix(p2, here); break;
        case DO: p1 = here; c(x->o1); c(x->o2); fix(hole(JNZ), p1); break;
        case EMPTY: break;
        case SEQ: c(x->o1); c(x->o2); break;
        case EXPR: c(x->o1); g(IDROP); break;
        case FUNC_CALL: if (x->val == 0) { error("undefined function!"); }
            g(ICALL); g2(x->val); break;
        case FUNC_DEF: c(x->o1); break;
        case PROG: c(x->o1); g(HALT);  break;
        case RET: g(IRET); break;
    }
}

/*---------------------------------------------------------------------------*/
/* Definitions. */

void defSize(int type, int sym) {
    // check for ";" or "[" <int> "];"
    symbols[sym].sz = (type == INT_TOK) ? 4 : 1;
    if (tok == SEMI) { next_token(); return; }
    expect_token(LARR);
    symbols[sym].sz = int_val;
    if (type == INT_TOK) { symbols[sym].sz *= 4; }
    expect_token(INT);
    expect_token(RARR);
    expect_token(SEMI);
}

node *defs(node *st) {
    node *x = st;
    next_token();
    while (1) {
        if (tok == EOI) { break; }
        if (tok == LBRA) { break; }
        if (tok == VOID_TOK) {
            int seqNo = 1;
            next_token(); expect_token(FUNC_TOK);
            int sym = genSymbol(id_name, FUNC_TOK);
            symbols[sym].val = here;
            x = gen(FUNC_DEF, NULL, NULL);
            x->sval = sym;
            if (tok != LBRA) error("'{' expected.");
            x->o1 = statement();
            c(x);
            g(IRET);
            continue;
        }
        if (tok == INT_TOK) {
            next_token(); expect_token(ID);
            int sym = genSymbol(id_name, VAR);
            defSize(INT_TOK, sym);
            continue;
        }
        if (tok == BYTE_TOK) {
            next_token(); expect_token(ID);
            int sym = genSymbol(id_name, VAR);
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
    char *fn = (argc > 1) ? argv[1] : "test.tc";
    input_fp = fopen(fn, "rt");
    if (!input_fp) { error("can't open source file"); }
    printf("compiling %s ... ", fn);
    initVM();
    g(NOP);
    defs(NULL);
    fclose(input_fp);
    input_fp = NULL;
    printf("%d code bytes (%d nodes)\n\n", here, num_nodes);

    int entryPoint = symbols[genSymbol("main", FUNC_TOK)].val;
    if (0 < entryPoint) { runVM(entryPoint); }
    else { error("no main() function!"); }
    dis();
    for (int i = 0; i < numSymbols; i++) {
        SYM_T *s = &symbols[i];
        printf("%s = %ld\n", s->name, s->val);
    }
    printf("\n");
    return 0;
}
