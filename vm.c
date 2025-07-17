/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include "tc.h"

long globals[26], sp, rsp;
code vm[1000];

#define ACASE    goto again; case
#define BCASE    break; case
#define TOS      stk[sp]
#define NOS      stk[sp-1]

static int  x2(int a) { return vm[a] | (vm[a + 1] << 8); }
static long x4(int a) {
    return vm[a+0] | (vm[a+1] << 8) | (vm[a+2] << 16) | (vm[a+3] << 24);
}

static long stk[0x80];
static int rstk[1000];

static void push(long x) { sp = (sp+1)&0x7f; TOS = x; }
static void drop() { sp = (sp-1)&0x7f; }
static long pop() { long x = TOS; drop(); return x; }

void initVM() {
    for (int i = 0; i < 26; i++) { globals[i] = 0; }
    sp = rsp = 0;
}

void runVM(int pc) {
    again:
    switch (vm[pc++]) {
        case  IFETCH: stk[++sp] = symbols[x2(pc)].val; pc += 2;
        ACASE ISTORE: symbols[x2(pc)].val = stk[sp]; pc += 2;
        ACASE IP1: push(vm[pc++]);
        ACASE IP2: push(x2(pc)); pc += 2;
        ACASE IP4: push(x4(pc)); pc += 4;
        ACASE IDROP: drop();
        ACASE IADD: NOS += TOS; drop();
        ACASE ISUB: NOS -= TOS; drop();
        ACASE IMUL: NOS *= TOS; drop();
        ACASE IDIV: NOS /= TOS; drop();
        ACASE ILT: NOS = (NOS < TOS) ? 1 : 0; drop();
        ACASE IGT: NOS = (NOS > TOS) ? 1 : 0; drop();
        ACASE IEQ: NOS = (NOS == TOS) ? 1 : 0; drop();
        ACASE JMP: pc = x2(pc);
        ACASE JZ:  if (pop() == 0) pc = x2(pc); else pc += 2;
        ACASE JNZ: if (pop() != 0) pc = x2(pc); else pc += 2;
        ACASE ICALL: rstk[rsp++] = pc+2; pc = x2(pc);
        ACASE IRET: if (rsp) { pc = rstk[--rsp]; } else { return; }
        ACASE HALT: return;
        default: printf("Invalid IR: %d", vm[pc-1]);  return;
    }
}

/*---------------------------------------------------------------------------*/
/* Disassembly. */

static FILE *outFp;
static void pB (int n) { for (int i=0; i<n; i++) fprintf(outFp, " "); }
static void pS(char *s) { fprintf(outFp, "; %s ", s); }
static void pSy(int t) { fprintf(outFp, "- (%s)", symbols[t].name); }
static void pSv(int v) { int t=findSymbolVal(0,v); pSy(t); }
static void pNX(long n) { fprintf(outFp, "%02lx ", n); }
static void pN1(int n) { pNX((n & 0xff)); }
static void pN2(int n) { pN1(n); pN1(n >> 8); }
static void pN4(int n) { pN2(n); pN2(n >> 16); }

void dis(int here) {
    code pc = 1;
    long t;
    outFp = fopen("list.txt", "wt");
    hDump(0, outFp);
    fprintf(outFp, "\n");
    dumpSymbols(1, outFp);

    fprintf(outFp, "\ncode: %d bytes (0x%x)\n----------------------------", here, here);
    while (pc < here) {
        fprintf(outFp, "\n%04x: %02d ", pc, vm[pc]);
        switch (vm[pc++]) {
            case  NOP:    pB(12); pS("nop");
            BCASE IFETCH: pN2(x2(pc)); pB(6); pS("fetch"); t = x2(pc); pNX(t); pSy(t); pc += 2;
            BCASE ISTORE: pN2(x2(pc)); pB(6); pS("store"); t = x2(pc); pNX(t); pSy(t); pc += 2;
            BCASE IP1:    pN1(x2(pc)); pB(9); pS("lit1");  pNX(vm[pc++]);
            BCASE IP2:    pN2(x2(pc)); pB(6); pS("lit2");  pNX(x2(pc)); pc += 2;
            BCASE IP4:    pN4(x4(pc)); pS("lit4");  pNX(x4(pc)); pc += 4;
            BCASE IDROP:  pB(12); pS("drop");
            BCASE IADD:   pB(12); pS("add");
            BCASE ISUB:   pB(12); pS("sub");
            BCASE IMUL:   pB(12); pS("mul");
            BCASE IDIV:   pB(12); pS("div");
            BCASE ILT:    pB(12); pS("lt");
            BCASE IGT:    pB(12); pS("gt");
            BCASE JMP:    pN2(x2(pc)); pB(6); pS("jmp");  pNX(x2(pc)); pc += 2;
            BCASE JZ:     pN2(x2(pc)); pB(6); pS("jz");   pNX(x2(pc)); pc += 2;
            BCASE JNZ:    pN2(x2(pc)); pB(6); pS("jnz");  pNX(x2(pc)); pc += 2;
            BCASE ICALL:  pN2(x2(pc)); pB(6); pS("call"); t = x2(pc); pNX(t); pSv(t); pc += 2;
            BCASE IRET:   pB(12); pS("ret");
            BCASE HALT:   pB(12); pS("halt"); break;
            default:      pB(12); pS("<invalid>");
        }
    }
    fprintf(outFp, "\n");
    fclose(outFp);
    outFp = NULL;
}

#ifdef _MAIN
int main() {
    return 0;
}
#endif // _MAIN
