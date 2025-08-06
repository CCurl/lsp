/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdio.h>
#include <stdint.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

int32_t reg[8];
int32_t EAX, EBX, ECX, EDX;
int32_t eip, esp, ebp, esi, edi;
int zf;

#define EAX reg[0]
#define ECX reg[1]
#define EDX reg[2]
#define EBX reg[3]


// VM opcodes
enum {
    NOP,
};


#define VM_SZ 100000
byte vm[VM_SZ];
int here;

#define ACASE    goto again; case
#define BCASE    break; case

static void    s4(int32_t a, int32_t v) { *(int32_t*)(&vm[a]) = v; }
static int32_t f1(int32_t a) { return vm[a]; }
static int32_t f4(int32_t a) { return *(int32_t*)(&vm[a]); }

void initVM() {
    esp = 0x0FFF;
    here = 0;
}

#define C2 (vm[eip]<<8) + vm[eip+1]

void ModRM(int op, int modrm) {

}

void runVM(int st) {
    eip = st;
    again:
    // printf("-pc:%04x/ir:%d-\n", pc, vm[pc]);
    switch (C2) {
        case  0x83c5: ebp = ebp+f1(eip+2); eip += 3; // add ebp,<b>
        ACASE 0x83ed: ebp = ebp-f1(eip+2); eip += 3; // sub ebp,<b>
        ACASE 0x89d8: EAX=EBX;  eip += 2; // sub ebp,<b>
        ACASE 0x89c3: EBX=EAX;  eip += 2; // sub ebp,<b>
        ACASE 0x01d8: EAX+=EBX; eip += 2; // add EAX, EBX
        ACASE 0x01c3: EBX+=EAX; eip += 2; // add EBX, EAX
        goto again;
    }
    switch (vm[eip]) {
        case  0x90: //                        // nop
        ACASE 0x50: esp -= 4; s4(esp, EAX);   // push EAX
        ACASE 0x51: esp -= 4; s4(esp, ECX);   // push ECX
        ACASE 0x52: esp -= 4; s4(esp, EDX);   // push EDX
        ACASE 0x53: esp -= 4; s4(esp, EBX);   // push EBX
        ACASE 0x58: EAX = f4(esp); esp += 4;  // pop EAX
        ACASE 0x5b: ECX = f4(esp); esp += 4;  // pop ECX
        ACASE 0x59: EDX = f4(esp); esp += 4;  // pop EDX
        ACASE 0x5a: EBX = f4(esp); esp += 4;  // pop EBX
        ACASE 0x9a: eip = f4(eip);            // call absolute
        ACASE 0xc3: eip = f4(esp); esp += 4;  // ret
        ACASE 0xb8: EAX = f4(eip); eip += 4;  // mov EAX, <imm>
        ACASE 0xbb: ECX = f4(eip); eip += 4;  // mov ECX, <imm>
        ACASE 0xb9: EDX = f4(eip); eip += 4;  // mov EDX, <imm>
        ACASE 0xba: EBX = f4(eip); eip += 4;  // mov EBX, <imm>
        default: printf("Invalid IR: %d\n", vm[eip-1]);  return;
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
            case  0x90:   pB(12); pS("nop");
            BCASE 0xb8:   pN4(f4(pc)); pB(6); pS("mov EAX, %d"); t = f4(pc); pNX(t); pc += 2;
            BCASE 0xc3:   pB(12); pS("ret");
            // BCASE ISTORE: pN2(f2(pc)); pB(6); pS("store"); t = f2(pc); pNX(t); pc += 2;
            // BCASE ILIT:    pN4(f4(pc)); pS("lit4");  pNX(f4(pc)); pc += 4;
            // BCASE IDROP:  pB(12); pS("drop");
            // BCASE IADD:   pB(12); pS("add");
            // BCASE ISUB:   pB(12); pS("sub");
            // BCASE IMUL:   pB(12); pS("mul");
            // BCASE IDIV:   pB(12); pS("div");
            // BCASE ILT:    pB(12); pS("lt");
            // BCASE IGT:    pB(12); pS("gt");
            // BCASE IEQ:    pB(12); pS("eq");
            // BCASE JZ:     pN2(f2(pc)); pB(6); pS("jz");   pNX(f2(pc)); pc += 2;
            // BCASE JNZ:    pN2(f2(pc)); pB(6); pS("jnz");  pNX(f2(pc)); pc += 2;
            // BCASE JMP:    pN2(f2(pc)); pB(6); pS("jmp");  pNX(f2(pc)); pc += 2;
            // BCASE ICALL:  pN2(f2(pc)); pB(6); pS("call"); t = f2(pc); pNX(t); pc += 2;
            // BCASE IRET:   pB(12); pS("ret");
            // BCASE HALT:   pB(12); pS("halt"); break;
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
        fp = fopen("vm-lin.lst", "wt");
        dis(fp);
        fclose(fp);
        runVM(0);
    }
    return 0;
}
