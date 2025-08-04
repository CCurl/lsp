/*---------------------------------------------------------------------------*/
/* Virtual machine. */

#include <stdio.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

/*---------------------------------------------------------------------------*/
/* HEX dump. */
void hexDump(byte *start, int count, FILE *fpOut) {
    int tt = 0;
    FILE *fp = fpOut ? fpOut : stdout;
    fprintf(fp, "\nHEX dump - %d bytes", count);
    fprintf(fp, "\n------------------------------------------------------");
    for (int i=0; i<count; i++) {
        if (tt==0) { fprintf(fp, "\n%04X: ", i); }
        byte x = start[i];
        fprintf(fp, "%02X ", x);
        if (++tt == 16) {
            tt = 0;
            fprintf(fp, "  ; ");
            for (int j=i-15; j<i; j++) {
                x = start[j];
                fprintf(fp, "%c", BTWI(x,32,126) ? x : '.');
            }
        }
    }
    fprintf(fp, "\n");
}

/*---------------------------------------------------------------------------*/
/* Main program. */
byte buf[65536];

int main(int argc, char *argv[]) {
    char *fn = (argc > 1) ? argv[1] : "tc.out";
    FILE *fp = fopen(fn, "rb");
    if (!fp) { printf("can't open file"); }
    else {
        int sz = (int)fread(buf, 1, sizeof(buf), fp);
        fclose(fp);
        hexDump(buf, sz, 0);
    }
    return 0;
}
