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

static pB(FILE* fp, int n) { for (int i=0; i<n; i++) fprintf(fp, " "); }
static pS(FILE *fp, char *s) { fprintf(fp, "; %s ", s); }
static pNX(FILE* fp, long n) { fprintf(fp, "%02lx ", n); }
static pN1(FILE* fp, int n) { pNX(fp, (n & 0xff)); }
static pN2(FILE* fp, int n) { pN1(fp, n); pN1(fp, n >> 8); }
static pN4(FILE *fp, int n) { pN2(fp, n); pN2(fp, n >> 16); }

void dis(int here) {
    code pc = 1;
    FILE *fp = fopen("list.txt", "wt");
    hDump(0, fp);
    fprintf(fp, "\n");
    dumpSymbols(1, fp);

    fprintf(fp, "\ncode: %d bytes (0x%x)\n----------------------------", here, here);
    while (pc < here) {
        fprintf(fp, "\n%04x: %02d ", pc, vm[pc]);
        switch (vm[pc++]) {
            case  NOP:    pB(fp,12); pS(fp, "nop");
            BCASE IFETCH: pN2(fp, x2(pc)); pB(fp,6); pS(fp, "fetch"); pNX(fp, x2(pc)); pc += 2;
            BCASE ISTORE: pN2(fp, x2(pc)); pB(fp,6); pS(fp, "store"); pNX(fp, x2(pc)); pc += 2;
            BCASE IP1:    pN1(fp, x2(pc)); pB(fp,9); pS(fp, "lit1");  pNX(fp, vm[pc++]);
            BCASE IP2:    pN2(fp, x2(pc)); pB(fp,6); pS(fp, "lit2");  pNX(fp, x2(pc)); pc += 2;
            BCASE IP4:    pN4(fp, x4(pc)); pS(fp, "lit4");  pNX(fp, x4(pc)); pc += 4;
            BCASE IDROP:  pB(fp,12); pS(fp, "drop");
            BCASE IADD:   pB(fp,12); pS(fp, "add");
            BCASE ISUB:   pB(fp,12); pS(fp, "sub");
            BCASE IMUL:   pB(fp,12); pS(fp, "mul");
            BCASE IDIV:   pB(fp,12); pS(fp, "div");
            BCASE ILT:    pB(fp,12); pS(fp, "lt");
            BCASE IGT:    pB(fp,12); pS(fp, "gt");
            BCASE JMP:    pN2(fp, x2(pc)); pB(fp,6); pS(fp, "jmp");  pNX(fp, x2(pc)); pc += 2;
            BCASE JZ:     pN2(fp, x2(pc)); pB(fp,6); pS(fp, "jz");   pNX(fp, x2(pc)); pc += 2;
            BCASE JNZ:    pN2(fp, x2(pc)); pB(fp,6); pS(fp, "jnz");  pNX(fp, x2(pc)); pc += 2;
            BCASE ICALL:  pN2(fp, x2(pc)); pB(fp,6); pS(fp, "call"); pNX(fp, x2(pc)); pc += 2;
            BCASE IRET:   pB(fp,12); pS(fp, "ret");
            BCASE HALT:   pB(fp,12); pS(fp, "halt"); break;
            default:      pB(fp, 12); pS(fp, "<invalid>");
        }
    }
    fprintf(fp, "\n");
    fclose(fp);
}

#ifdef _MAIN
int main() {
    return 0;
}
#endif // _MAIN
