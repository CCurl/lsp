/*---------------------------------------------------------------------------*/
/* hex-dump. */

#include <stdio.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

/*---------------------------------------------------------------------------*/
/* HEX dump. */
void hexDump(byte *start, int count, int dets) {
    int n = 0;
    printf( "\nHEX dump - %d bytes", count);
    printf( "\n-------------------------------------------------------");
    for (int i=0; i<count; i=i+16) {
        printf( "\n");
        if (dets) { printf( "%04X: ", i); }
        for (int j=0; j<8; j++) { printf( "%02X ", start[i+j]); }
        printf( " ");
        for (int j=8; j<16; j++) { printf( "%02X ", start[i+j]); }
        if (dets == 0) { continue; }
        printf( "  ; ");
        for (int j=0; j<16; j++) {
            char x = start[i+j];
            printf( "%c", BTWI(x,32,126) ? x : '.');
        }
    }
    printf( "\n");
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
        hexDump(buf, sz, dets);
    }
    return 0;
}
