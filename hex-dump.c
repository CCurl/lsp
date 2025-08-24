/*---------------------------------------------------------------------------*/
/* hex-dump. */

#include <stdio.h>

#define BTWI(n,l,h) ((l<=n)&&(n<=h))
typedef unsigned char byte;

/*---------------------------------------------------------------------------*/
/* HEX dump. */
void hexDump(byte *start, int count, int dets, char *pfx) {
    int n = 0;
    printf( "\n%sHEX dump - %d bytes", pfx, count);
    printf( "\n%s-------------------------------------------------------", pfx);
    for (int i=0; i<count; i=i+16) {
        printf( "\n%s", pfx);
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
    char *fn = (argc > 1) ? argv[1] : "undefined";
    char *pfx = "";
    if ((argc > 2) && (fn[0] == '_') && (fn[1] == 0)) {
        pfx = "; ";
        dets = 0;
        fn = argv[2];
    }
    FILE *fp = fopen(fn, "rb");
    if (!fp) { printf("can't open file (%s)", fn); }
    else {
        int sz = (int)fread(buf, 1, sizeof(buf), fp);
        fclose(fp);
        hexDump(buf, sz, dets, pfx);
    }
    return 0;
}
