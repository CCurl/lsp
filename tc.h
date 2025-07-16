#ifndef __TC_H__
#define __TC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

typedef unsigned char byte;
typedef char code;

enum {
    IFETCH, ISTORE, IP1, IP2, IP4, IDROP, IADD, ISUB, IMUL, IDIV,
    ILT, IGT, IEQ, JZ, JNZ, JMP, ICALL, IRET, HALT
};

extern long globals[];
extern code vm[];
extern void initVM();
extern void runVM(code *pc);
extern void dis(code *pc, code *here);
extern void dumpSymbols(int details, FILE *toFP);

#endif // __TC_H__
