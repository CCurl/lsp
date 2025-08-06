/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdio.h>
#include <stdint.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;
typedef void (*voidfn_t)();

int32_t reg[8], ip, ir. zf;

#define EAX reg[0]
#define ECX reg[1]
#define EDX reg[2]
#define EBX reg[3]
#define ESP reg[4]
#define EBP reg[5]
#define ESI reg[6]
#define EDI reg[7]

#define VM_SZ 100000
byte vm[VM_SZ];
int here;

#define ACASE    goto again; case
#define BCASE    break; case
#define RCASE    return; case

static void    s1(int32_t a, int32_t v) { vm[a] = (v&0xff); }
static void    s4(int32_t a, int32_t v) { *(int32_t*)(&vm[a]) = v; }
static int32_t f1(int32_t a) { return vm[a]; }
static int32_t f4(int32_t a) { return *(int32_t*)(&vm[a]); }

void initVM() {
    ESP = VM_SZ;
    eip = 0;
    here = 0;
}

/*    /r     */
void SlashR(int mod, int r, int m) {
    int d = 0;                    // Displacement
    int32_t *src = NULL, *tgt = NULL, disp = 0;
    switch (op) {
        case 0x01: *tgt += *src;  return;  // ADD
        case 0x89: *tgt  = *src;  return;  // MOV
    }
}

/*     ModR/M    */
void ModRM(int op, int modrm) {
    int modrm = f1(ip++);
    int mod = (modrm >> 6) & 0x03;  // ModRM: bits 6-7 - mode
    int r   = (modrm >> 3) & 0x07;  // ModRM: bits 3-5 - reg
    int m   = (modrm >> 0) & 0x07;  // ModRM: bits 0-2 - reg/mem
    int d   = 0;                    // Displacement

    // printf("ModRM: mod=%d r=%d m=%d disp=%d\n", mod, r, m, d);

    int32_t *src = NULL, *tgt = NULL, disp = 0;

    switch (mod) {
        case 0: /* TODO */ // memory, no displacement
            break;
        case 1: /* TODO */ // 8-bit displacement
            break;
        case 2: /* TODO */ // 32-bit displacement
            break;
        case 3: src = &reg[r], tgt = &reg[m];  // register to register
            break;
    }

    switch (op) {
        case 0x01: *tgt += *src;  return;  // ADD
        case 0x89: *tgt  = *src;  return;  // MOV
    }
}

void runVM(int st) {
    ip = st;
    again:
    ir = (f1(ip)<<8) + fi(ip+1);
    // printf("-pc:%04x/ir:%d-\n", pc, vm[pc]);
    switch (ir) {
        case  0x83c5: EBP = EBP+f1(ip+2); ip += 3; // add EBP,<b>
        ACASE 0x83ed: EBP = EBP-f1(ip+2); ip += 3; // sub EBP,<b>
        ACASE 0x89d8: EAX=EBX;  ip += 2;  // sub EBP,<b>
        ACASE 0x89c3: EBX=EAX;  ip += 2;  // sub EBP,<b>
        ACASE 0x01d8: EAX+=EBX; ip += 2;  // add EAX, EBX
        ACASE 0x01c3: EBX+=EAX; ip += 2;  // add EBX, EAX
        goto again;
    }
    ir = vm[ip++];
    switch (ir) {
        case  0x90: //                        // nop
        ACASE 0x50: ESP -= 4; s4(ESP, EAX);   // push EAX
        ACASE 0x51: ESP -= 4; s4(ESP, ECX);   // push ECX
        ACASE 0x52: ESP -= 4; s4(ESP, EDX);   // push EDX
        ACASE 0x53: ESP -= 4; s4(ESP, EBX);   // push EBX
        ACASE 0x58: EAX = f4(ESP); ESP += 4;  // pop EAX
        ACASE 0x5b: ECX = f4(ESP); ESP += 4;  // pop ECX
        ACASE 0x59: EDX = f4(ESP); ESP += 4;  // pop EDX
        ACASE 0x5a: EBX = f4(ESP); ESP += 4;  // pop EBX
        ACASE 0x9a: ip = f4(ip);            // call absolute
        ACASE 0xc3: ip = f4(ESP); ESP += 4;  // ret
        ACASE 0xb8: EAX = f4(ip); ip += 4;  // mov EAX, <imm>
        ACASE 0xbb: ECX = f4(ip); ip += 4;  // mov ECX, <imm>
        ACASE 0xb9: EDX = f4(ip); ip += 4;  // mov EDX, <imm>
        ACASE 0xba: EBX = f4(ip); ip += 4;  // mov EBX, <imm>
        default: printf("Invalid IR: %d\n", vm[ip-1]);  return;
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

void undefinedOp() { printf("Undefined opcode: %d\n", ir); }

void op0() { undefinedOp(); }
void op1() { undefinedOp(); }
void op2() { undefinedOp(); }
void op3() { undefinedOp(); }
void op4() { undefinedOp(); }
void op5() { undefinedOp(); }
void op6() { undefinedOp(); }
void op7() { undefinedOp(); }
void op8() { undefinedOp(); }
void op9() { undefinedOp(); }
void op10() { undefinedOp(); }
void op11() { undefinedOp(); }
void op12() { undefinedOp(); }
void op13() { undefinedOp(); }
void op14() { undefinedOp(); }
void op15() { undefinedOp(); }
void op16() { undefinedOp(); }
void op17() { undefinedOp(); }
void op18() { undefinedOp(); }
void op19() { undefinedOp(); }
void op20() { undefinedOp(); }
void op21() { undefinedOp(); }
void op22() { undefinedOp(); }
void op23() { undefinedOp(); }
void op24() { undefinedOp(); }
void op25() { undefinedOp(); }
void op26() { undefinedOp(); }
void op27() { undefinedOp(); }
void op28() { undefinedOp(); }
void op29() { undefinedOp(); }
void op30() { undefinedOp(); }
void op31() { undefinedOp(); }
void op32() { undefinedOp(); }
void op33() { undefinedOp(); }
void op34() { undefinedOp(); }
void op35() { undefinedOp(); }
void op36() { undefinedOp(); }
void op37() { undefinedOp(); }
void op38() { undefinedOp(); }
void op39() { undefinedOp(); }
void op40() { undefinedOp(); }
void op41() { undefinedOp(); }
void op42() { undefinedOp(); }
void op43() { undefinedOp(); }
void op44() { undefinedOp(); }
void op45() { undefinedOp(); }
void op46() { undefinedOp(); }
void op47() { undefinedOp(); }
void op48() { undefinedOp(); }
void op49() { undefinedOp(); }
void op50() { undefinedOp(); }
void op51() { undefinedOp(); }
void op52() { undefinedOp(); }
void op53() { undefinedOp(); }
void op54() { undefinedOp(); }
void op55() { undefinedOp(); }
void op56() { undefinedOp(); }
void op57() { undefinedOp(); }
void op58() { undefinedOp(); }
void op59() { undefinedOp(); }
void op60() { undefinedOp(); }
void op61() { undefinedOp(); }
void op62() { undefinedOp(); }
void op63() { undefinedOp(); }
void op64() { undefinedOp(); }
void op65() { undefinedOp(); }
void op66() { undefinedOp(); }
void op67() { undefinedOp(); }
void op68() { undefinedOp(); }
void op69() { undefinedOp(); }
void op70() { undefinedOp(); }
void op71() { undefinedOp(); }
void op72() { undefinedOp(); }
void op73() { undefinedOp(); }
void op74() { undefinedOp(); }
void op75() { undefinedOp(); }
void op76() { undefinedOp(); }
void op77() { undefinedOp(); }
void op78() { undefinedOp(); }
void op79() { undefinedOp(); }
void op80() { undefinedOp(); }
void op81() { undefinedOp(); }
void op82() { undefinedOp(); }
void op83() { undefinedOp(); }
void op84() { undefinedOp(); }
void op85() { undefinedOp(); }
void op86() { undefinedOp(); }
void op87() { undefinedOp(); }
void op88() { undefinedOp(); }
void op89() { undefinedOp(); }
void op90() { undefinedOp(); }
void op91() { undefinedOp(); }
void op92() { undefinedOp(); }
void op93() { undefinedOp(); }
void op94() { undefinedOp(); }
void op95() { undefinedOp(); }
void op96() { undefinedOp(); }
void op97() { undefinedOp(); }
void op98() { undefinedOp(); }
void op99() { undefinedOp(); }
void op100() { undefinedOp(); }
void op101() { undefinedOp(); }
void op102() { undefinedOp(); }
void op103() { undefinedOp(); }
void op104() { undefinedOp(); }
void op105() { undefinedOp(); }
void op106() { undefinedOp(); }
void op107() { undefinedOp(); }
void op108() { undefinedOp(); }
void op109() { undefinedOp(); }
void op110() { undefinedOp(); }
void op111() { undefinedOp(); }
void op112() { undefinedOp(); }
void op113() { undefinedOp(); }
void op114() { undefinedOp(); }
void op115() { undefinedOp(); }
void op116() { undefinedOp(); }
void op117() { undefinedOp(); }
void op118() { undefinedOp(); }
void op119() { undefinedOp(); }
void op120() { undefinedOp(); }
void op121() { undefinedOp(); }
void op122() { undefinedOp(); }
void op123() { undefinedOp(); }
void op124() { undefinedOp(); }
void op125() { undefinedOp(); }
void op126() { undefinedOp(); }
void op127() { undefinedOp(); }
void op128() { undefinedOp(); }
void op129() { undefinedOp(); }
void op130() { undefinedOp(); }
void op131() { undefinedOp(); }
void op132() { undefinedOp(); }
void op133() { undefinedOp(); }
void op134() { undefinedOp(); }
void op135() { undefinedOp(); }
void op136() { undefinedOp(); }
void op137() { undefinedOp(); }
void op138() { undefinedOp(); }
void op139() { undefinedOp(); }
void op140() { undefinedOp(); }
void op141() { undefinedOp(); }
void op142() { undefinedOp(); }
void op143() { undefinedOp(); }
void op144() { undefinedOp(); }
void op145() { undefinedOp(); }
void op146() { undefinedOp(); }
void op147() { undefinedOp(); }
void op148() { undefinedOp(); }
void op149() { undefinedOp(); }
void op150() { undefinedOp(); }
void op151() { undefinedOp(); }
void op152() { undefinedOp(); }
void op153() { undefinedOp(); }
void op154() { undefinedOp(); }
void op155() { undefinedOp(); }
void op156() { undefinedOp(); }
void op157() { undefinedOp(); }
void op158() { undefinedOp(); }
void op159() { undefinedOp(); }
void op160() { undefinedOp(); }
void op161() { undefinedOp(); }
void op162() { undefinedOp(); }
void op163() { undefinedOp(); }
void op164() { undefinedOp(); }
void op165() { undefinedOp(); }
void op166() { undefinedOp(); }
void op167() { undefinedOp(); }
void op168() { undefinedOp(); }
void op169() { undefinedOp(); }
void op170() { undefinedOp(); }
void op171() { undefinedOp(); }
void op172() { undefinedOp(); }
void op173() { undefinedOp(); }
void op174() { undefinedOp(); }
void op175() { undefinedOp(); }
void op176() { undefinedOp(); }
void op177() { undefinedOp(); }
void op178() { undefinedOp(); }
void op179() { undefinedOp(); }
void op180() { undefinedOp(); }
void op181() { undefinedOp(); }
void op182() { undefinedOp(); }
void op183() { undefinedOp(); }
void op184() { undefinedOp(); }
void op185() { undefinedOp(); }
void op186() { undefinedOp(); }
void op187() { undefinedOp(); }
void op188() { undefinedOp(); }
void op189() { undefinedOp(); }
void op190() { undefinedOp(); }
void op191() { undefinedOp(); }
void op192() { undefinedOp(); }
void op193() { undefinedOp(); }
void op194() { undefinedOp(); }
void op195() { undefinedOp(); }
void op196() { undefinedOp(); }
void op197() { undefinedOp(); }
void op198() { undefinedOp(); }
void op199() { undefinedOp(); }
void op200() { undefinedOp(); }
void op201() { undefinedOp(); }
void op202() { undefinedOp(); }
void op203() { undefinedOp(); }
void op204() { undefinedOp(); }
void op205() { undefinedOp(); }
void op206() { undefinedOp(); }
void op207() { undefinedOp(); }
void op208() { undefinedOp(); }
void op209() { undefinedOp(); }
void op210() { undefinedOp(); }
void op211() { undefinedOp(); }
void op212() { undefinedOp(); }
void op213() { undefinedOp(); }
void op214() { undefinedOp(); }
void op215() { undefinedOp(); }
void op216() { undefinedOp(); }
void op217() { undefinedOp(); }
void op218() { undefinedOp(); }
void op219() { undefinedOp(); }
void op220() { undefinedOp(); }
void op221() { undefinedOp(); }
void op222() { undefinedOp(); }
void op223() { undefinedOp(); }
void op224() { undefinedOp(); }
void op225() { undefinedOp(); }
void op226() { undefinedOp(); }
void op227() { undefinedOp(); }
void op228() { undefinedOp(); }
void op229() { undefinedOp(); }
void op230() { undefinedOp(); }
void op231() { undefinedOp(); }
void op232() { undefinedOp(); }
void op233() { undefinedOp(); }
void op234() { undefinedOp(); }
void op235() { undefinedOp(); }
void op236() { undefinedOp(); }
void op237() { undefinedOp(); }
void op238() { undefinedOp(); }
void op239() { undefinedOp(); }
void op240() { undefinedOp(); }
void op241() { undefinedOp(); }
void op242() { undefinedOp(); }
void op243() { undefinedOp(); }
void op244() { undefinedOp(); }
void op245() { undefinedOp(); }
void op246() { undefinedOp(); }
void op247() { undefinedOp(); }
void op248() { undefinedOp(); }
void op249() { undefinedOp(); }
void op250() { undefinedOp(); }
void op251() { undefinedOp(); }
void op252() { undefinedOp(); }
void op253() { undefinedOp(); }
void op254() { undefinedOp(); }
void op255() { undefinedOp(); }

voidfn_t opcodes[256] = {
    op0, op1, op2, op3, op4, op5, op6, op7,
    op8, op9, op10, op11, op12, op13, op14, op15,
    op16, op17, op18, op19, op20, op21, op22, op23,
    op24, op25, op26, op27, op28, op29, op30, op31,
    op32, op33, op34, op35, op36, op37, op38, op39,
    op40, op41, op42, op43, op44, op45, op46, op47,
    op48, op49, op50, op51, op52, op53, op54, op55,
    op56, op57, op58, op59, op60, op61, op62, op63,
    op64, op65, op66, op67, op68, op69, op70, op71,
    op72, op73, op74, op75, op76, op77, op78, op79,
    op80, op81, op82, op83, op84, op85, op86, op87,
    op88, op89, op90, op91, op92, op93, op94, op95,
    op96, op97, op98, op99, op100, op101, op102, op103,
    op104, op105, op106, op107, op108, op109, op110, op111,
    op112, op113, op114, op115, op116, op117, op118, op119,
    op120, op121, op122, op123, op124, op125, op126, op127,
    op128, op129, op130, op131, op132, op133, op134, op135,
    op136, op137, op138, op139, op140, op141, op142, op143,
    op144, op145, op146, op147, op148, op149, op150, op151,
    op152, op153, op154, op155, op156, op157, op158, op159,
    op160, op161, op162, op163, op164, op165, op166, op167,
    op168, op169, op170, op171, op172, op173, op174, op175,
    op176, op177, op178, op179, op180, op181, op182, op183,
    op184, op185, op186, op187, op188, op189, op190, op191,
    op192, op193, op194, op195, op196, op197, op198, op199,
    op200, op201, op202, op203, op204, op205, op206, op207,
    op208, op209, op210, op211, op212, op213, op214, op215,
    op216, op217, op218, op219, op220, op221, op222, op223,
    op224, op225, op226, op227, op228, op229, op230, op231,
    op232, op233, op234, op235, op236, op237, op238, op239,
    op240, op241, op242, op243, op244, op245, op246, op247,
    op248, op249, op250, op251, op252, op253, op254, op255,
};

int32_t runOPcode(int op) {
    if (BTWI(op, 0, 255)) {
        opcodes[op]();
    } else {
        printf("Invalid opcode: %d\n", op);
    }
}

void runCpu(int st) {
    ip = st;
    while (ip < here) {
        ir = f1(ip++);
        runOPcode(ir);
    }
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
