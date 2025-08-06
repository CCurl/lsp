#include <stdio.h>

int org;
char vmIn[100000];
char vmOut[100000];
int here, cHere, iHere;

// ----------------------------------------------------------------
void cc1(int c) { vmOut[here++] = (c&0xff); cHere++; }
void cc2(int c) { cc1(c); cc1(c>>8); }
void cc4(int c) { cc2(c); cc2(c>>16); }

void ccN(int n, const char *str) {
    for (int i=0; i<n; i++) { cc1(str[i]); }
}

int hole(int c) {
    cc1(c);
    int h = here;
    cc4(0);
    return h;
}

void fix(int tgt, int val) {
    int h = here;
    here = tgt;
    cc4(val);
    here = h;
}

// ----------------------------------------------------------------
// Quasi standard library routines
int defAlloc256() {
    int h = cHere;
    ccN(5, "\xb8\x2d\x00\x00\x00");     // mov  eax,0x2d
    ccN(2, "\x31\xdb");                 // xor  ebx,ebx
    ccN(2, "\xcd\x80");                 // int  0x80
    ccN(2, "\x89\xc3");                 // mov  ebx,eax
    ccN(1, "\x58");                     // pop  eax
    ccN(6, "\x81\xc3\x00\x01\x00\x00"); // add  ebx,0x100
    ccN(5, "\xb8\x2d\x00\x00\x00");     // mov  eax,0x2d
    ccN(2, "\xcd\x80");                 // int  0x80
    ccN(1, "\xc3");                     // ret  (addr is in eax)
    return h;
}

int defPutC() {
    int h = cHere;
    // syscall WRITE - EAX=$04, EBX=<nchars>, ECX=<buf>, EDX=<sz>
    // TODO: get char, put it it somehere, and set ecx
    ccN(5, "\xb8\x04\x00\x00\x00");    // mov eax,$04 (write)
    ccN(5, "\xbb\x01\x00\x00\x00");    // mov ebx,$01
    ccN(2, "\xcd\x80");                // int 0x80
    ccN(1, "\xc3");                    // ret
    return h;
}

void defStdLib() {
    defPutC();
    defAlloc256();
}

// ----------------------------------------------------------------

void elfHeader() {
    // ELF header
    ccN(16, "\x7f\x45\x4c\x46\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    ccN(16, "\x02\x00\x03\x00\x01\x00\x00\x00\x54\x80\x04\x08\x34\x00\x00\x00");
    ccN(16, "\x00\x00\x00\x00\x00\x00\x00\x00\x34\x00\x20\x00\x01\x00\x00\x00");
    ccN(16, "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x80\x04\x08");
    ccN(16, "\x00\x80\x04\x08\x10\x4b\x00\x00\x10\x4b\x00\x00\x07\x00\x00\x00");
    ccN(16, "\x00\x10\x00\x00\xe8\x00\x00\x00\x00\x89\xc3\x31\xc0\x40\xcd\x80");
}

void outputVM() {
    for (int i = 0; i < here; i++) { putchar(vmOut[i]); }
}

void readVmIn() {
    iHere = 0;
    int c = getchar();
    while (c != EOF) {
        vmIn[iHere++] = c;
        c = getchar();
    }
}

int main() {
    readVmIn();
    here = cHere = 0;
    elfHeader();

    org = 0x08048000; // linux 23-bit code start (134512640)
    cHere = org;
    int mainTgt = hole(0xe9);
    
    defStdLib();
    fix(mainTgt, cHere);

    ccN(5, "\xb8\x78\x78\x78\x78");
 
    // WRITE - EAX=$04, EBX=<nchars>, ECX=<buf>, EDX=<sz>
    ccN(5, "\xb8\x04\x00\x00\x00"); // MOV eax,$04 (write)
    ccN(5, "\xbb\x01\x00\x00\x00"); // MOV ebx,$01
    ccN(2, "\xcd\x80");             // INT $80
    
    // exit - EAX=$01, EBX=<code>
    ccN(5, "\xb8\x01\x00\x00\x00"); // MOV eax,$01 (exit)
    ccN(5, "\xbb\x00\x0a\x00\x00"); // MOV ebx,$10
    ccN(2, "\xcd\x80");             // INT $80
 
    ccN(5, "\xb8\x78\x78\x78\x78");

    outputVM();
}
