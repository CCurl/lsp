/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdio.h>
#include <stdint.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

// VM opcodes
enum {
    NOP, IFETCH, ISTORE, IP1, IP2, IP4, IDROP, IADD, ISUB, IMUL, IDIV,
    ILT, IGT, IEQ, JZ, JNZ, JMP, ICALL, IRET, HALT
};


#define VM_SZ 10000
byte vm[VM_SZ];
int here;
static long stk[0x80], sp;
static long rstk[1000], rsp;
static long vals[1000];

#define ACASE    goto again; case
#define BCASE    break; case
#define TOS      stk[sp]
#define NOS      stk[sp-1]

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
        ACASE IFETCH: stk[++sp] = vals[f2(pc)]; pc += 2;
        ACASE ISTORE: vals[f2(pc)] = stk[sp--]; pc += 2;
        ACASE IP1: stk[++sp] = vm[pc++];
        ACASE IP2: stk[++sp] = f2(pc); pc += 2;
        ACASE IP4: stk[++sp] = f4(pc); pc += 4;
        ACASE IDROP: --sp;
        ACASE IADD: NOS += TOS; --sp;
        ACASE ISUB: NOS -= TOS; --sp;
        ACASE IMUL: NOS *= TOS; --sp;
        ACASE IDIV: NOS /= TOS; --sp;
        ACASE ILT: NOS = (NOS < TOS) ? 1 : 0; --sp;
        ACASE IGT: NOS = (NOS > TOS) ? 1 : 0; --sp;
        ACASE IEQ: NOS = (NOS == TOS) ? 1 : 0; --sp;
        ACASE JZ:  if (stk[sp--] == 0) pc = f2(pc); else pc += 2;
        ACASE JNZ: if (stk[sp--] != 0) pc = f2(pc); else pc += 2;
        ACASE JMP: pc = f2(pc);
        ACASE ICALL: rstk[rsp++] = pc+2; pc = f2(pc);
        ACASE IRET: if (rsp) { pc = rstk[--rsp]; } else { return; }
        ACASE HALT: return;
        default: printf("Invalid IR: %d\n", vm[pc-1]);  return;
    }
}

/*---------------------------------------------------------------------------*/
/* Disassembly. */

static FILE *outFp;
static void pB(int n) { for (int i=0; i<n; i++) fprintf(outFp, " "); }
static void pS(char *s) { fprintf(outFp, "; %s ", s); }
static void pNX(long n) { fprintf(outFp, "%02lX ", n); }
static void pN1(int n) { pNX((n & 0xff)); }
static void pN2(int n) { pN1(n); pN1(n >> 8); }
static void pN4(int n) { pN2(n); pN2(n >> 16); }

void dis(FILE *toFp) {
    int pc = 0;
    long t;
    outFp = toFp ? toFp : stdout;

    fprintf(outFp, "\nVM: %d bytes, %d used, 0x0000:0x%04X.", VM_SZ, here, here-1);
    fprintf(outFp, "\n-------------------------------------------");
    while (pc < here) {
        fprintf(outFp, "\n%04X: %02X ", pc, vm[pc]);
        switch (vm[pc++]) {
            case  NOP:    pB(12); pS("nop");
            BCASE IFETCH: pN2(f2(pc)); pB(6); pS("fetch"); t = f2(pc); pNX(t); pc += 2;
            BCASE ISTORE: pN2(f2(pc)); pB(6); pS("store"); t = f2(pc); pNX(t); pc += 2;
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
            BCASE ICALL:  pN2(f2(pc)); pB(6); pS("call"); t = f2(pc); pNX(t); pc += 2;
            BCASE IRET:   pB(12); pS("ret");
            BCASE HALT:   pB(12); pS("halt"); break;
            default:      pB(12); pS("<invalid>");
        }
    }
    fprintf(outFp, "\n");
}

/*---------------------------------------------------------------------------*/
/* Main program. */

int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : "tc.out";
    FILE *fp = fopen(fn, "rb");
    initVM();
    if (!fp) { printf("can't open program"); }
    else {
        here = (int)fread(vm, 1, VM_SZ, fp);
        fclose(fp);
        fp = fopen("vm-stk.lst", "wt");
        dis(fp);
        fclose(fp);
        runVM(0);
        for (int i=0; i<1000; i++) {
            if (vals[i] != 0) { printf("%d: %ld\n", i, vals[i]); }
        }
    }
    return 0;
}
