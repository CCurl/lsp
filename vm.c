/*---------------------------------------------------------------------------*/
// A register-based virtual machine.
// This is (will be) an extremely stripped down 32-bit Linux/x86 emulator.

#include <stdio.h>
#include <stdint.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

// VM opcodes
enum {
    NOP, IFETCH, ISTORE, ILIT, IDROP
    , ILT, IGT, IEQ, INEQ, ILAND, ILOR, ILNOT
    , IADD, ISUB, IMUL, IDIV
    , IAND, IOR, IXOR
    , JZ, JNZ, JMP
    , ICALL=0xe8, IRET=0xc3
    , MovRR=0x89, MovIMM=0xb8, MovFet=0xa1, MovSto=0xa3
};

#define VM_SZ 10000

byte vm[VM_SZ];
int here;
static long stk[128];
static long vals[1000];
static long EAX, EBX, ECX, EDX, ESP, EBP, ESI, EDI;
static long EIP;

#define rAX 0
#define rCX 1
#define rDX 2
#define rBX 3
#define rSP 4
#define rBP 5
#define rSI 6
#define rDI 7
#define R2R(rT, rF) ((3<<6) | (rF<<3) | rT)

#define ACASE    goto again; case
#define BCASE    break; case

static int  f1(int a) { return *(int8_t*)(&vm[a]); }
static int  f2(int a) { return *(int16_t*)(&vm[a]); }
static int  f4(int a) { return *(int32_t*)(&vm[a]); }

void initVM() {
    ESP = here = 0;
}

static void mov(int what) {
    switch (what) {
        case R2R(rDX, rCX): EDX = ECX;  break;
        case R2R(rCX, rBX): ECX = EBX;  break;
        case R2R(rBX, rAX): EBX = EAX;  break;
        case R2R(rAX, rBX): EAX = EBX;  break;
        case R2R(rBX, rCX): EBX = ECX;  break;
        case R2R(rCX, rDX): ECX = EDX;  break;
    }
}

static void sPush() {
    // EDX = ECX; ECX = EBX; EBX = EAX;
    // mov(R2R(rDX, rCX));
    mov(R2R(rCX, rBX));
    mov(R2R(rBX, rAX));
}

static void sPop() {
    // EAX = EBX; EBX = ECX; ECX = EDX;
    mov(R2R(rAX, rBX));
    mov(R2R(rBX, rCX));
    //mov(R2R(rCX, rDX));
}

static void push(long x) { stk[++ESP] = x;  }
static int  pop() { return stk[ESP--]; }

void runVM(int st) {
    byte ir;
    EIP = st;
    again:
    // if (maxSp < sp) { maxSp = sp; }
    // printf("-EIP:%04x/ir:%d-\n", EIP, vm[EIP]);
    ir = vm[EIP++];
    switch (ir) {
        case  NOP:
        // ACASE IFETCH: sPush(); EAX = vals[f2(EIP)]; EIP += 2;
        // ACASE ISTORE: vals[f2(EIP)] = EAX; sPop(); EIP += 2;
        ACASE ILIT:  sPush(); EAX = f4(EIP); EIP += 4;
        ACASE IDROP: sPop();
        ACASE ILT:   if (EBX <  EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE IGT:   if (EBX >  EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE IEQ:   if (EBX == EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE INEQ:  if (EBX != EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILAND: if (EBX && EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILOR:  if (EBX || EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILNOT: if (EAX == 0) { EAX = 1; } else { EAX = 0; }
        ACASE IADD:  EBX = (EBX + EAX); sPop();
        ACASE ISUB:  EBX = (EBX - EAX); sPop();
        ACASE IMUL:  EBX = (EBX * EAX); sPop();
        ACASE IDIV:  EBX = (EBX / EAX); sPop();
        ACASE IAND:  EBX = (EBX & EAX); sPop();
        ACASE IOR:   EBX = (EBX | EAX); sPop();
        ACASE IXOR:  EBX = (EBX ^ EAX); sPop();
        ACASE JZ:    if (EAX == 0) { EIP = f2(EIP); } else { EIP += 2; } sPop();
        ACASE JNZ:   if (EAX != 0) { EIP = f2(EIP); } else { EIP += 2; } sPop();
        ACASE JMP:   EIP = f2(EIP);
        ACASE ICALL: push(EIP+4); EIP = f4(EIP);
        ACASE IRET: if (ESP < 1) { return; } EIP = pop();
        // ACASE HALT: return;
        ACASE MovRR: mov(vm[EIP++]);
        ACASE MovIMM: EAX = f4(EIP); EIP += 4;
        ACASE MovFet: sPush(); EAX = vals[f4(EIP)]; EIP += 4;
        ACASE MovSto: vals[f4(EIP)] = EAX; sPop(); EIP += 4;
        goto again; default: 
            printf("Invalid IR: %d at 04%lX\n", ir, EIP-1);  return;
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

static void pRR(int a) {
    int t = a & 3;
    if (t == 0) { fprintf(outFp, "EAX, "); }
    else if (t == 1) { fprintf(outFp, "ECX, "); }
    else if (t == 2) { fprintf(outFp, "EDX, "); }
    else if (t == 3) { fprintf(outFp, "EBX, "); }

    t = (a>>3) & 3;
    if (t == 0) { fprintf(outFp, "EAX "); }
    else if (t == 1) { fprintf(outFp, "ECX"); }
    else if (t == 2) { fprintf(outFp, "EDX"); }
    else if (t == 3) { fprintf(outFp, "EBX"); }
}

void dis(FILE *toFp) {
    EIP = 0;
    long t;
    outFp = toFp ? toFp : stdout;

    fprintf(outFp, "\nVM: %d bytes, %d used, 0x0000:0x%04X.", VM_SZ, here, here-1);
    fprintf(outFp, "\n-------------------------------------------");
    while (EIP < here) {
        fprintf(outFp, "\n%04lX: %02X ", EIP, vm[EIP]);
        switch (vm[EIP++]) {
            case  NOP:    pB(12); pS("nop");
            BCASE IFETCH: pN2(f2(EIP)); pB(6); pS("fetch"); t = f2(EIP); pNX(t); EIP += 2;
            BCASE ISTORE: pN2(f2(EIP)); pB(6); pS("store"); t = f2(EIP); pNX(t); EIP += 2;
            BCASE ILIT:   pN4(f4(EIP)); pS("lit4");  pNX(f4(EIP)); EIP += 4;
            BCASE IDROP:  pB(12); pS("drop");
            BCASE ILT:    pB(12); pS("lt");
            BCASE IGT:    pB(12); pS("gt");
            BCASE IEQ:    pB(12); pS("eq");
            BCASE INEQ:   pB(12); pS("neq");
            BCASE ILAND:  pB(12); pS("land");
            BCASE ILOR:   pB(12); pS("lor");
            BCASE ILNOT:  pB(12); pS("lnot");
            BCASE IADD:   pB(12); pS("add");
            BCASE ISUB:   pB(12); pS("sub");
            BCASE IMUL:   pB(12); pS("mul");
            BCASE IDIV:   pB(12); pS("div");
            BCASE IAND:   pB(12); pS("and");
            BCASE IOR:    pB(12); pS("or");
            BCASE IXOR:   pB(12); pS("xor");
            BCASE JZ:     pN2(f2(EIP)); pB(6); pS("jz");   pNX(f2(EIP)); EIP += 2;
            BCASE JNZ:    pN2(f2(EIP)); pB(6); pS("jnz");  pNX(f2(EIP)); EIP += 2;
            BCASE JMP:    pN2(f2(EIP)); pB(6); pS("jmp");  pNX(f2(EIP)); EIP += 2;
            BCASE ICALL:  t=f4(EIP); pN4(t);   pS("call"); pNX(t);  EIP += 4;
            BCASE IRET:   pB(12); pS("ret");
            // BCASE HALT:   pB(12); pS("halt");
            BCASE MovRR:  t=f1(EIP); pN1(t); pB(9); pS("mov "); pRR(t); EIP += 1;
            BCASE MovFet: t=f4(EIP); pN4(t); pB(0); pS("fetch from"); pNX(t); EIP += 4;
            BCASE MovSto: t=f4(EIP); pN4(t); pB(0); pS("store to");   pNX(t); EIP += 4;
            BCASE MovIMM: t=f4(EIP); pN4(t); pB(0); pS("mov eax,");  pNX(t); EIP += 4;
            break; default: pB(12); pS("<invalid>");
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
        fp = fopen("vm.lst", "wt");
        dis(fp);
        fclose(fp);
        runVM(0);
        for (int i=0; i<1000; i++) {
            if (vals[i] != 0) { printf("%d: %ld\n", i, vals[i]); }
        }
        // printf("max-sp: %ld\n", maxSp);
    }
    return 0;
}
