/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdio.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

/*---------------------------------------------------------------------------*/
/* HEX dump. */
void hexDump(byte *start, int count, FILE *fpOut, int dets) {
    int n = 0;
    FILE *fp = fpOut ? fpOut : stdout;
    fprintf(fp, "\nHEX dump - %d bytes", count);
    fprintf(fp, "\n-------------------------------------------------------");
    for (int i=0; i<count; i=i+16) {
        fprintf(fp, "\n");
        if (dets) { fprintf(fp, "%04X: ", i); }
        for (int j=0; j<8; j++) { fprintf(fp, "%02X ", start[i+j]); }
        fprintf(fp, " ");
        for (int j=8; j<16; j++) { fprintf(fp, "%02X ", start[i+j]); }
        if (dets == 0) { continue; }
        fprintf(fp, "  ; ");
        for (int j=0; j<16; j++) {
            byte x = start[i+j];
            fprintf(fp, "%c", BTWI(x,32,126) ? x : '.');
        }
    }
    fprintf(fp, "\n");
}

/*---------------------------------------------------------------------------*/
/* Main program. */
byte buf[65536];

int main(int argc, char *argv[]) {
    int fni = 1;
    int dets = 1;
    if ((argc > 1) && (argv[1][0] == '_')) { dets = 0; ++fni; }
    char *fn = (argc > fni) ? argv[fni] : "tc.out";
    FILE *fp = fopen(fn, "rb");
    if (!fp) { printf("can't open file"); }
    else {
        int sz = (int)fread(buf, 1, sizeof(buf), fp);
        fclose(fp);
        hexDump(buf, sz, 0, dets);
    }
    return 0;
}
