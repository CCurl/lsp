/*---------------------------------------------------------------------------*/
// A register-based virtual machine.
// This is (will be) an extremely stripped down 32-bit Linux/x86 emulator.

#include <stdio.h>
#include <stdint.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

// VM opcodes
enum {
    NOP, IADD=0x01
    , ILT=0x50, IGT, IEQ, INEQ, ILAND, ILOR, ILNOT
    , JZ, JNZ, JMP
    , IAND=0x21, IOR=0x09, IXOR=0x31
    , ISUB=0x29, MULDIV=0xf7
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

static int f1(int a) { return vm[a]; }
static int f2(int a) { return *(int16_t*)(&vm[a]); }
static int f4(int a) { return *(int32_t*)(&vm[a]); }
static int ip1() { int t=f1(EIP); EIP += 1; return t; }
static int ip2() { int t=f2(EIP); EIP += 2; return t; }
static int ip4() { int t=f4(EIP); EIP += 4; return t; }

void initVM() {
    ESP = here = 0;
}

static void MOV(int what) {
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
    // MOV(R2R(rDX, rCX));
    MOV(R2R(rCX, rBX));
    MOV(R2R(rBX, rAX));
}

static void sPop() {
    // EAX = EBX; EBX = ECX; ECX = EDX;
    MOV(R2R(rAX, rBX));
    MOV(R2R(rBX, rCX));
    //MOV(R2R(rCX, rDX));
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
        ACASE ILT:   if (EBX <  EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE IGT:   if (EBX >  EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE IEQ:   if (EBX == EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE INEQ:  if (EBX != EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILAND: if (EBX && EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILOR:  if (EBX || EAX) { EBX = 1; } else { EBX = 0; } sPop();
        ACASE ILNOT: if (EAX == 0) { EAX = 1; } else { EAX = 0; }
        ACASE IADD:  if (ip1()==0xd8) { EAX += EBX; }
        ACASE ISUB:  if (ip1()==0xc3) { EBX -= EAX; }
        ACASE MULDIV: ir=ip1();
                      if (ir==0xeb) { EAX *= EBX; }
                      if (ir==0xfb) { EAX /= EBX; }
        ACASE IAND:  if (ip1()==0xd8) { EAX &= EBX; }
        ACASE IOR:   if (ip1()==0xd8) { EAX |= EBX; }
        ACASE IXOR:  if (ip1()==0xd8) { EAX ^= EBX; }
        ACASE JZ:    if (EAX == 0) { EIP = f2(EIP); } else { EIP += 2; } sPop();
        ACASE JNZ:   if (EAX != 0) { EIP = f2(EIP); } else { EIP += 2; } sPop();
        ACASE JMP:   EIP = f2(EIP);
        ACASE ICALL: push(EIP+4); EIP = f4(EIP);
        ACASE IRET: if (ESP < 1) { return; } EIP = pop();
        ACASE MovRR: MOV(vm[EIP++]);
        ACASE MovIMM: EAX = ip4();
        ACASE MovFet: EAX = vals[ip4()];
        ACASE MovSto: vals[ip4()] = EAX;
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

static void pR(int r) {
    r = r&3;
    if (r == 0) { fprintf(outFp, "EAX"); }
    else if (r == 1) { fprintf(outFp, "ECX"); }
    else if (r == 2) { fprintf(outFp, "EDX"); }
    else if (r == 3) { fprintf(outFp, "EBX"); }
}

static void pRR(int a) {
    pR(a & 3);
    fprintf(outFp, ", "),
    pR((a>>3) & 3);
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
            BCASE ILT:    pB(12); pS("lt");
            BCASE IGT:    pB(12); pS("gt");
            BCASE IEQ:    pB(12); pS("eq");
            BCASE INEQ:   pB(12); pS("neq");
            BCASE ILAND:  pB(12); pS("land");
            BCASE ILOR:   pB(12); pS("lor");
            BCASE ILNOT:  pB(12); pS("lnot");
            BCASE IADD:   t=ip1(); pN1(t); pB(9); pS("ADD"); pRR(t);
            BCASE ISUB:   t=ip1(); pN1(t); pB(9); pS("SUB"); pRR(t);
            BCASE MULDIV: t=ip1(); pN1(t); pB(9);
                          if (t==0xeb) { fprintf(outFp, "; IMUL EBX"); }
                          if (t==0xfb) { fprintf(outFp, "; IDIV EBX"); }
            BCASE IAND:   t=ip1(); pN1(t); pB(9); pS("AND"); pRR(t);
            BCASE IOR:    t=ip1(); pN1(t); pB(9); pS("OR"); pRR(t);
            BCASE IXOR:   t=ip1(); pN1(t); pB(9); pS("XOR"); pRR(t);
            BCASE JZ:     t=ip2(); pN2(t); pB(6); fprintf(outFp, "; JMPZ 0x%04lx", t);
            BCASE JNZ:    t=ip2(); pN2(t); pB(6); fprintf(outFp, "; JMPNZ 0x%04lx", t);
            BCASE JMP:    t=ip2(); pN2(t); pB(6); fprintf(outFp, "; JMP 0x%04lx", t);
            BCASE ICALL:  t=ip4(); pN4(t);        pS("call"); pNX(t);
            BCASE IRET:   pB(12); pS("RET");
            BCASE MovRR:  t=ip1(); pN1(t); pB(9); pS("MOV"); pRR(t);
            BCASE MovFet: t=ip4(); pN4(t); fprintf(outFp, "; MOV EAX, [0x%08lx]", t);
            BCASE MovSto: t=ip4(); pN4(t); fprintf(outFp, "; MOV [0x%08lx], EAX", t);
            BCASE MovIMM: t=ip4(); pN4(t); fprintf(outFp, "; MOV EAX, 0x%08lx", t);
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
