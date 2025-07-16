/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include "tc.h"

long globals[26], sp, rsp;
code vm[1000];

#define ACASE    goto again; case
#define TOS      stk[sp]
#define NOS      stk[sp-1]

static long f4(byte *a) {
    int x = *(a + 3);
    x = (x << 8) | *(a + 2);
    x = (x << 8) | *(a + 1);
    x = (x << 8) | *(a + 0);
    return x;
}

static int f2(byte *a) {
    int x = *(a + 1);
    x = (x << 8) | *(a + 0);
    return x;
}

static long stk[0x80];
static code *rstk[1000];

static void push(long x) { sp = (sp+1)&0x7f; TOS = x; }
static void drop() { sp = (sp-1)&0x7f; }
static long pop() { long x = TOS; drop(); return x; }

void initVM() {
    sp = rsp = 0;
}

void runVM(code *pc) {
    again:
    switch (*pc++) {
        case  IFETCH: stk[++sp] = globals[f2((byte*)pc)]; pc += 2;
        ACASE ISTORE: globals[f2((byte*)pc)] = stk[sp]; pc += 2;
        ACASE IP1: push(*(pc++));
        ACASE IP2: push(f2((byte*)pc)); pc += 2;
        ACASE IP4: push(f4((byte*)pc)); pc += 4;
        ACASE IDROP: drop();
        ACASE IADD: NOS += TOS; drop();
        ACASE ISUB: NOS -= TOS; drop();
        ACASE IMUL: NOS *= TOS; drop();
        ACASE IDIV: NOS /= TOS; drop();
        ACASE ILT: NOS = (NOS < TOS) ? 1 : 0; drop();
        ACASE IGT: NOS = (NOS > TOS) ? 1 : 0; drop();
        ACASE IEQ: NOS = (NOS == TOS) ? 1 : 0; drop();
        ACASE JMP: pc += *pc;
        ACASE JZ:  if (pop() == 0) pc += *pc; else pc++;
        ACASE JNZ: if (pop() != 0) pc += *pc; else pc++;
        ACASE ICALL: /* rst[rsp++] = pc+2; pc=f2((byte*)pc); */
            printf("-run:call %d-\n", f2((byte*)pc)); pc += 2;
        ACASE IRET: if (rsp) { pc = rstk[--rsp]; }
            else { printf("-return?-"); } return;
        ACASE HALT: return;
    }
}

/*---------------------------------------------------------------------------*/
/* Disassembly. */

void dis(code *here) {
    code *pc = &vm[0];
    FILE *fp = fopen("list.txt", "wt");
    hDump(0, fp);
    fprintf(fp, "\n");
    dumpSymbols(1, fp);

again:
    if (here <= pc) { return; }
    int p = (int)(pc - &vm[0]);
    fprintf(fp, "\n%04d: %02d ; ", p++, *pc);
    switch (*pc++) {
        case  IFETCH: fprintf(fp, "fetch %d", f2((byte*)pc)); pc += 2;
        ACASE ISTORE: fprintf(fp, "store %d", f2((byte*)pc)); pc += 2;
        ACASE IP1:    fprintf(fp, "lit1 %d", *(pc++));
        ACASE IP2:    fprintf(fp, "lit2 %d", f2((byte*)pc)); pc += 2;
        ACASE IP4:    fprintf(fp, "lit4 %ld", f4((byte*)pc)); pc += 4;
        ACASE IDROP:  fprintf(fp, "drop");
        ACASE IADD:   fprintf(fp, "add");
        ACASE ISUB:   fprintf(fp, "sub");
        ACASE IMUL:   fprintf(fp, "mul");
        ACASE IDIV:   fprintf(fp, "div");
        ACASE ILT:    fprintf(fp, "lt");
        ACASE IGT:    fprintf(fp, "gt");
        ACASE JMP:    fprintf(fp, "jmp %d", p + *pc); pc++;
        ACASE JZ:     fprintf(fp, "jz  %d", p + *pc); pc++;
        ACASE JNZ:    fprintf(fp, "jnz %d", p + *pc); pc++;
        ACASE ICALL:  fprintf(fp, "call %d", f2((byte*)pc)); pc += 2;
        ACASE IRET:   fprintf(fp, "ret");
        ACASE HALT:   fprintf(fp, "halt");
    }
    fprintf(fp, "\n");
    fclose(fp);
}

#ifdef _MAIN
int main() {
    return 0;
}
#endif // _MAIN
