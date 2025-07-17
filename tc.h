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
typedef byte code;

typedef struct { char type, *name; long val; } SYM_T;
extern SYM_T symbols[SYMBOLS_SZ];

enum {
    NOP, IFETCH, ISTORE, IP1, IP2, IP4, IDROP, IADD, ISUB, IMUL, IDIV,
    ILT, IGT, IEQ, JZ, JNZ, JMP, ICALL, IRET, HALT
};

extern code vm[CODE_SZ];
extern void initVM();
extern void runVM(int pc);
extern void dis(int here);
extern int findSymbolVal(char type, long val);
extern void dumpSymbols(int details, FILE *toFP);

#endif // __TC_H__
