#ifndef __TC_H__
#define __TC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#define SYMBOLS_SZ   500
#define CODE_SZ     2500
#define NODES_SZ    1000

#define BTWI(n,l,h) ((l<=n)&&(n<=h))

typedef unsigned char byte;

typedef struct { char type, *name; int sz; long val; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

// Tokens - NOTE: the first 8 must match the words list in tc.c
enum {
    DO_TOK, ELSE_TOK, IF_TOK, WHILE_TOK
    , VOID_TOK, INT_TOK, BYTE_TOK, RET_TOK
    , LBRA, RBRA, LPAR, RPAR, LARR, RARR
    , PLUS, MINUS, STAR, SLASH, LESS, EQU, GRT, SEMI
    , EQUAL, INT, ID, EOI, FUNC_TOK
};

// Syntax tree node types
enum {
    VAR, CST, ADD, SUB, MUL, DIV, LT, EQ, GT, SET, FUNC_CALL, FUNC_DEF,
    IF1, IF2, WHILE, DO, EMPTY, SEQ, EXPR, PROG, RET
};

// VM opcodes
enum {
    NOP, IFETCH, ISTORE, IP1, IP2, IP4, IDROP, IADD, ISUB, IMUL, IDIV,
    ILT, IGT, IEQ, JZ, JNZ, JMP, ICALL, IRET, HALT
};

extern int here;
extern byte vm[CODE_SZ];
extern void initVM();
extern void runVM(int pc);
extern void dis();
extern int findSymbolVal(char type, long val);
extern void dumpSymbols(int details, FILE *toFP);

#endif // __TC_H__
