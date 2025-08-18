/*---------------------------------------------------------------------------*/
// A register-based virtual machine.
// This is (will be) an extremely stripped down 32-bit Linux/x86 emulator.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VM_SZ      100000
#define STK_SZ         63
#define VALS_SZ       256

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

// #define DBG
#ifdef DBG
FILE *trcf;
void startDBG() { trcf = fopen("vm.trc","wt"); }
void stopDBG() { fclose(trcf); }
#else
void startDBG() {}
void stopDBG() {}
#endif

// VM/x86 opcodes
enum {
    // These are real
    NOP=0x90, IADD=0x01
    , IAND=0x21, IOR=0x09, IXOR=0x31
    , ISUB=0x29, MULDIV=0xf7
    , IRET=0xc3
    , MovRR=0x89, MovIMM=0xb8, MovFet=0xa1, MovSto=0xa3
    , XCHGAB=0x93, ICMP=0x39
    , JZ=0x74, JNZ=0x75, JGE=0x7d, JLE=0x7e
    , INCDX=0x42, DECDX=0x4a
    // These are not real
    , ILAND=0x60, JMPZ, JMPNZ
};

byte vm[VM_SZ];
static int32_t here, *vals;
static int32_t regs[8], stk[STK_SZ+1];
static long EIP, t;

#define rAX 0
#define rCX 1
#define rDX 2
#define rBX 3
#define rSP 4
#define rBP 5
#define rSI 6
#define rDI 7

#define EAX regs[rAX]
#define ECX regs[rCX]
#define EDX regs[rDX]
#define EBX regs[rBX]
#define ESP regs[rSP]
#define EBP regs[rBP]
#define ESI regs[rSI]
#define EDI regs[rDI]

#define ACASE    goto again; case
#define BCASE    break; case
#define RCASE    return; case

static int f1(int a) { return vm[a]; }
static int f2(int a) { return *(int16_t*)(&vm[a]); }
static int f4(int a) { return *(int32_t*)(&vm[a]); }
static int ip1() { int t=f1(EIP); EIP += 1; return t; }
static int ip2() { int t=f2(EIP); EIP += 2; return t; }
static int ip4() { int t=f4(EIP); EIP += 4; return t; }
static void s1(long a, long v) { vm[a] = (v&0xff); }
static void s4(long a, long v) { *(int32_t*)(&vm[a]) = v; }

void initVM(int sz) {
    ESP = 0;
    here = sz;
    while (sz&3) { ++sz; }
    vals = (int32_t*)&vm[sz];
}

static void MOV(int what) {
    int rT = what & 0x07;
    int rF = (what>>3) & 0x07;
    regs[rT] = regs[rF];
}

static void push(long x) { stk[++ESP] = x;  }
static int  pop() { return stk[ESP--]; }

byte ZF;  // Zero-Flag
byte SF;  // Sign-Flag
byte CF;  // Carry-Flag

void setFlags(int val) {
    ZF = (val == 0) ? 1 : 0;
    SF = (val <  0) ? 1 : 0;
    CF = SF;
}

void doCmp(int ir) {
    if (ir == 0xc3) { setFlags(EBX - EAX); }
}

// Linux simulation
void sys01() { stopDBG(); exit(EBX); }
void sys02() {  }
void sys04() { for (int i=0; i<EDX; i++ ) { putchar(vm[ECX+i]); } }
void sys45() {  } // sys_brk
void doInt(int num) {
    if (num != 0x80) { return; }
    switch (EAX) {
        case   1: sys01();
        RCASE  2: sys02();
        RCASE  4: sys04();
        RCASE 45: sys45();
        return; default: printf("INT 80: unknown call %d", EAX);
    }
}

void runVM(int st) {
    byte ir;
    EIP = st;
    again:
    // if (maxSp < sp) { maxSp = sp; }
    #ifdef DBG
    fprintf(trcf, "EIP:%04lX, ir:%02X, ", EIP, vm[EIP]);
    fprintf(trcf, "ESP:%2d, EAX:%d, TOS:%d, EBX:%d, EDX:%d\n",
        ESP, EAX, stk[ESP], EBX, EDX);
    if (!BTWI(ESP, 0, STK_SZ)) { return; }
    #endif
    ir = vm[EIP++];
    switch (ir) {
        case  NOP:
        ACASE 0x50:   push(EAX);
        ACASE 0x51:   push(ECX);
        ACASE 0x52:   push(EDX);
        ACASE 0x53:   push(EBX);
        ACASE 0x54:   push(ESP);
        ACASE 0x55:   push(EBP);
        ACASE 0x56:   push(ESI);
        ACASE 0x57:   push(EDI);
        ACASE 0x58:   EAX = pop();
        ACASE 0x59:   ECX = pop();
        ACASE 0x5a:   EDX = pop();
        ACASE 0x5b:   EBX = pop();
        ACASE 0x5c:   ESP = pop();
        ACASE 0x5d:   EBP = pop();
        ACASE 0x5e:   ESI = pop();
        ACASE 0x5f:   EDI = pop();
        ACASE IADD:   if (ip1()==0xd8) { EAX += EBX; }  // 0x01
        ACASE ISUB:   if (ip1()==0xd8) { EAX -= EBX; }  // 0x29
        ACASE MULDIV: ir = ip1();                       // 0xf7
                      if (ir==0xeb) { EAX *= EBX; }
                      else if (ir==0xfb) { EAX /= EBX; }
        ACASE IAND:   if (ip1()==0xd8) { EAX &= EBX; }     // 0x21
        ACASE IOR:    if (ip1()==0xd8) { EAX |= EBX; }     // 0x09
        ACASE IXOR:   ir = ip1();                          // 0x31
                      if (ir==0xd8) { EAX ^= EBX; }
                      else if (ir==0xd2) { EDX ^= EDX; }
        ACASE INCDX:  EDX++; setFlags(EDX);                // 0x42
        ACASE DECDX:  EDX--; setFlags(EDX);                // 0x4A
        ACASE ICMP:   ir=ip1(); doCmp(ir);                 // 0x39
        ACASE JZ:     ir=ip1(); if (ZF)            { EIP += (char)ir; }  // 0x74
        ACASE JNZ:    ir=ip1(); if (!ZF)           { EIP += (char)ir; }  // 0x75
        ACASE JGE:    ir=ip1(); if ((ZF) || (!SF)) { EIP += (char)ir; }  // 0x7d
        ACASE JLE:    ir=ip1(); if ((ZF) || (SF))  { EIP += (char)ir; }  // 0x7e
        ACASE IRET:   if (ESP < 1) { return; } EIP = pop();  // 0xc3
        ACASE MovRR:  MOV(ip1());
        ACASE MovIMM: EAX = ip4();
        ACASE MovFet: EAX = vals[ip4()];
        ACASE MovSto: vals[ip4()] = EAX;
        ACASE 0x85:   ir=ip1(); if (ir == 0xc0) { setFlags(EAX); }         // TEST EAX, EAX
        ACASE 0x8d:   ir=ip1(); t=ip4(); if (ir == 0x15) { EDX = t; }      // LEA EDX, [addr]
        ACASE XCHGAB: t=EAX; EAX=EBX; EBX=t;
        ACASE 0xcd:   doInt(ip1());                                        // INT
        ACASE 0xff:   ir=ip1();
                      if (ir == 0xd2) { push(EIP); EIP = EDX; }            // CALL EDX
                      else if (ir == 0x15) { push(EIP+4); EIP = ip4(); }   // CALL [addr]
                      else if (ir == 0x25) { EIP = ip4(); }                // JMP [addr]
        goto again; default: 
            printf("Invalid IR: %02X at %04lX\n", ir, EIP-1);  return;
    }
}

/*---------------------------------------------------------------------------*/
/* Disassembly. */

static FILE *outFp;
static void pB(int n) { for (int i=0; i<(n*3); i++) fprintf(outFp, " "); }
static void pS(char *s) { fprintf(outFp, "; %s ", s); }
static void pNX(long n) { fprintf(outFp, "%02lX ", n); }
static void pN1(int n) { pNX((n & 0xff)); }
static void pN2(int n) { pN1(n); pN1(n >> 8); }
static void pN4(int n) { pN2(n); pN2(n >> 16); }

static void pR(int r) {
    r = r&7;
    if (r == 0) { fprintf(outFp, "EAX"); }
    else if (r == 1) { fprintf(outFp, "ECX"); }
    else if (r == 2) { fprintf(outFp, "EDX"); }
    else if (r == 3) { fprintf(outFp, "EBX"); }
    else if (r == 4) { fprintf(outFp, "ESP"); }
    else if (r == 5) { fprintf(outFp, "EBP"); }
    else if (r == 6) { fprintf(outFp, "ESI"); }
    else if (r == 7) { fprintf(outFp, "EBI"); }
}

static void pRR(int a) {
    pR(a);
    fprintf(outFp, ", "),
    pR(a>>3);
}

void dis(FILE *toFp) {
    EIP = 0;
    outFp = toFp ? toFp : stdout;
    byte ir;

    fprintf(outFp, "\nVM: %d bytes, %d used, 0x0000:0x%04X.", VM_SZ, here, here-1);
    fprintf(outFp, "\n-------------------------------------------");
    while (EIP < here) {
        fprintf(outFp, "\n%04lX: %02X ", EIP, vm[EIP]);
        switch (vm[EIP++]) {
            case  NOP:    pB(5); pS("NOP");
            BCASE 0x42:   pB(5); fprintf(outFp, "; INC EDX");
            BCASE 0x4a:   pB(5); fprintf(outFp, "; DEC EDX");
            BCASE 0x50:   pB(5); fprintf(outFp, "; PUSH EAX");
            BCASE 0x51:   pB(5); fprintf(outFp, "; PUSH ECX");
            BCASE 0x52:   pB(5); fprintf(outFp, "; PUSH EDX");
            BCASE 0x53:   pB(5); fprintf(outFp, "; PUSH EBX");
            BCASE 0x54:   pB(5); fprintf(outFp, "; PUSH ESP");
            BCASE 0x55:   pB(5); fprintf(outFp, "; PUSH EBP");
            BCASE 0x56:   pB(5); fprintf(outFp, "; PUSH ESI");
            BCASE 0x57:   pB(5); fprintf(outFp, "; PUSH EDI");
            BCASE 0x58:   pB(5); fprintf(outFp, "; POP EAX");
            BCASE 0x59:   pB(5); fprintf(outFp, "; POP ECX");
            BCASE 0x5a:   pB(5); fprintf(outFp, "; POP EDX");
            BCASE 0x5b:   pB(5); fprintf(outFp, "; POP EBX");
            BCASE 0x5c:   pB(5); fprintf(outFp, "; POP ESP");
            BCASE 0x5d:   pB(5); fprintf(outFp, "; POP EBP");
            BCASE 0x5e:   pB(5); fprintf(outFp, "; POP ESI");
            BCASE 0x5f:   pB(5); fprintf(outFp, "; POP EDI");
            BCASE JNZ:    t=ip1(); pN1(t); pB(4); fprintf(outFp, "; JNZ %04lX", EIP+(char)t);
            BCASE JZ:     t=ip1(); pN1(t); pB(4); fprintf(outFp, "; JZ %04lX", EIP+(char)t);
            BCASE JGE:    t=ip1(); pN1(t); pB(4); fprintf(outFp, "; JGE %04lX", EIP+(char)t);
            BCASE JLE:    t=ip1(); pN1(t); pB(4); fprintf(outFp, "; JLE %04lX", EIP+(char)t);
            BCASE ICMP:   t=ip1(); pN1(t); pB(4); pS("CMP"); pRR(t);
            BCASE IADD:   t=ip1(); pN1(t); pB(4); pS("ADD"); pRR(t);
            BCASE ISUB:   t=ip1(); pN1(t); pB(4); pS("SUB"); pRR(t);
            BCASE MULDIV: t=ip1(); pN1(t); pB(4);
                          if (t==0xeb) { fprintf(outFp, "; IMUL EBX"); }
                          if (t==0xfb) { fprintf(outFp, "; IDIV EBX"); }
            BCASE IAND:   t=ip1(); pN1(t); pB(4); pS("AND"); pRR(t);
            BCASE IOR:    t=ip1(); pN1(t); pB(4); pS("OR"); pRR(t);
            BCASE IXOR:   t=ip1(); pN1(t); pB(4); pS("XOR"); pRR(t);
            BCASE IRET:   pB(5); pS("RET");
            BCASE MovRR:  t=ip1(); pN1(t); pB(4); pS("MOV"); pRR(t);
            BCASE MovFet: t=ip4(); pN4(t); pB(1); fprintf(outFp, "; MOV EAX, [0x%08lx]", t);
            BCASE MovSto: t=ip4(); pN4(t); pB(1); fprintf(outFp, "; MOV [0x%08lx], EAX", t);
            BCASE MovIMM: t=ip4(); pN4(t); pB(1); fprintf(outFp, "; MOV EAX, 0x%08lx (%ld)", t, t);
            BCASE XCHGAB: pB(5); pS("XCHG EAX, EBX");
            BCASE 0x85:   ir=ip1(); pB(5); if (ir == 0xc0) { fprintf(outFp, "; TEST EAX, EAX"); }
            BCASE 0x8d:   ir=ip1(); t=ip4(); pN1(ir); pN4(t); if (ir == 0x15) { fprintf(outFp, "; LEA EDX, [0x%08lx]", t); }
            BCASE 0xcd:   t=ip1(); pN1(t); pB(4); fprintf(outFp, "; INT 0x%02lx", t);
            BCASE 0xff:   ir=ip1(); pN1(ir);
                    if (ir == 0xd2) { pB(4); fprintf(outFp, "; CALL EDX"); }
                    if (ir == 0x15) { t=ip4(); pN4(t); pB(0); fprintf(outFp, "; CALL [0x%08lx]", t); }
                    if (ir == 0x25) { t=ip4(); pN4(t); pB(0); fprintf(outFp, "; JMP [0x%08lx]", t); }
            break; default: pB(5); pS("<invalid>");
        }
    }
    fprintf(outFp, "\n");
}

/*---------------------------------------------------------------------------*/
/* Main program. */

int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : "tc.out";
    FILE *fp = fopen(fn, "rb");
    startDBG();
    if (!fp) { printf("can't open program"); }
    else {
        int sz = (int)fread(vm, 1, VM_SZ, fp);
        initVM(sz);
        fclose(fp);
        fp = fopen("vm.lst", "wt");
        dis(fp);
        fclose(fp);
        runVM(0);
        for (int i=0; i<VALS_SZ; i++) {
            if (vals[i] != 0) { printf("%3d: %d\n", i, vals[i]); }
        }
        printf("ESP:%d, EAX:%d, EBX:%d, ECX:%d, EDX:%d\n",
             ESP, EAX, EBX, ECX, EDX);
    }

    stopDBG();
    return 0;
}
