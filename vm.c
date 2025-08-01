/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdint.h>
#include "tc.h"

byte vm[CODE_SZ];
int here;
static long stk[0x80], sp;
static long rstk[1000], rsp;

#define ACASE    goto again; case
#define BCASE    break; case
#define TOS      stk[sp]
#define NOS      stk[sp-1]

static void push(long x) { sp = (sp+1)&0x7f; TOS = x; }
static void drop() { sp = (sp-1)&0x7f; }
static long pop() { long x = TOS; drop(); return x; }
static int  f2(int a) { return *(int16_t*)(&vm[a]); }
static int  f4(int a) { return *(int32_t*)(&vm[a]); }

void initVM() {
    sp = rsp = here = 0;
}

void runVM(int pc) {
    again:
    // printf("-pc:%d/ir:%d-\n", pc, vm[pc]);
    switch (vm[pc++]) {
        case  NOP:
        ACASE IFETCH: stk[++sp] = symbols[f2(pc)].val; pc += 2;
        ACASE ISTORE: symbols[f2(pc)].val = stk[sp]; pc += 2;
        ACASE IP1: stk[++sp] = vm[pc++];
        ACASE IP2: stk[++sp] = f2(pc); pc += 2;
        ACASE IP4: stk[++sp] = f4(pc); pc += 4;
        ACASE IDROP: drop();
        ACASE IADD: NOS += TOS; drop();
        ACASE ISUB: NOS -= TOS; drop();
        ACASE IMUL: NOS *= TOS; drop();
        ACASE IDIV: NOS /= TOS; drop();
        ACASE ILT: NOS = (NOS < TOS) ? 1 : 0; drop();
        ACASE IGT: NOS = (NOS > TOS) ? 1 : 0; drop();
        ACASE IEQ: NOS = (NOS == TOS) ? 1 : 0; drop();
        ACASE JZ:  if (pop() == 0) pc = f2(pc); else pc += 2;
        ACASE JNZ: if (pop() != 0) pc = f2(pc); else pc += 2;
        ACASE JMP: pc = f2(pc);
        ACASE ICALL: rstk[rsp++] = pc+2; pc = f2(pc);
        ACASE IRET: if (rsp) { pc = rstk[--rsp]; } else { return; }
        ACASE HALT: return;
        default: printf("Invalid IR: %d\n", vm[pc-1]);  return;
    }
}

/*---------------------------------------------------------------------------*/
/* HEX dump. */
void hexDump(byte *start, int count, FILE *fpOut) {
    int tt = 0;
    FILE *fp = fpOut ? fpOut : stdout;
    fprintf(fp, "\nHEX dump");
    fprintf(fp, "\n------------------------------------------------------");
    for (int i=0; i<count; i++) {
        if (tt==0) { fprintf(fp, "\n%04X: ", i); }
        byte x = start[i];
        fprintf(fp, "%02X ", x);
        if (++tt == 16) {
            tt = 0;
            fprintf(fp, "  ; ");
            for (int j=i-15; j<i; j++) {
                x = start[j];
                fprintf(fp, "%c", BTWI(x,32,126) ? x : '.');
            }
        }
    }
    fprintf(fp, "\n");
}

/*---------------------------------------------------------------------------*/
/* Disassembly. */

static FILE *outFp;
static void pB(int n) { for (int i=0; i<n; i++) fprintf(outFp, " "); }
static void pS(char *s) { fprintf(outFp, "; %s ", s); }
static void pSy(int t) {
    if (BTWI(t,0,100)) {
        fprintf(outFp, "- (%s)", symbols[t].name);
    }
}
static void pSv(int v) { int t=findSymbolVal(FUNC_TOK,v); pSy(t); }
static void pNX(long n) { fprintf(outFp, "%02lX ", n); }
static void pN1(int n) { pNX((n & 0xff)); }
static void pN2(int n) { pN1(n); pN1(n >> 8); }
static void pN4(int n) { pN2(n); pN2(n >> 16); }

void dis() {
    int pc = 0;
    long t;
    outFp = fopen("listing.txt", "wt");
    hDump(0, outFp);
    fprintf(outFp, "\n");
    dumpSymbols(1, outFp);

    hexDump(&vm[0], here, outFp);
    fprintf(outFp, "\n");

    fprintf(outFp, "\ncode: %d bytes, %d (0x%X) used.", CODE_SZ, here, here);
    fprintf(outFp, "\n-------------------------------------------");
    while (pc < here) {
        fprintf(outFp, "\n%04X: %02X ", pc, vm[pc]);
        switch (vm[pc++]) {
            case  NOP:    pB(12); pS("nop");
            BCASE IFETCH: pN2(f2(pc)); pB(6); pS("fetch"); t = f2(pc); pNX(t); pSy(t); pc += 2;
            BCASE ISTORE: pN2(f2(pc)); pB(6); pS("store"); t = f2(pc); pNX(t); pSy(t); pc += 2;
            BCASE IP1:    pN1(f2(pc)); pB(9); pS("lit1");  pNX(vm[pc++]);
            BCASE IP2:    pN2(f2(pc)); pB(6); pS("lit2");  pNX(f2(pc)); pc += 2;
            BCASE IP4:    pN4(f4(pc)); pS("lit4");  pNX(f4(pc)); pc += 4;
            BCASE IDROP:  pB(12); pS("drop");
            BCASE IADD:   pB(12); pS("add");
            BCASE ISUB:   pB(12); pS("sub");
            BCASE IMUL:   pB(12); pS("mul");
            BCASE IDIV:   pB(12); pS("div");
            BCASE ILT:    pB(12); pS("lt");
            BCASE IGT:    pB(12); pS("gt");
            BCASE IEQ:    pB(12); pS("eq");
            BCASE JZ:     pN2(f2(pc)); pB(6); pS("jz");   pNX(f2(pc)); pc += 2;
            BCASE JNZ:    pN2(f2(pc)); pB(6); pS("jnz");  pNX(f2(pc)); pc += 2;
            BCASE JMP:    pN2(f2(pc)); pB(6); pS("jmp");  pNX(f2(pc)); pc += 2;
            BCASE ICALL:  pN2(f2(pc)); pB(6); pS("call"); t = f2(pc); pNX(t); pSv(t); pc += 2;
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
