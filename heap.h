#ifndef __HALLOC__

#define __HALLOC__

#include <stdio.h>
#include <stdint.h>

// Change these to control heap size and index

#define HEAPINDEX_SZ 500
#define HEAP_SZ 10000

extern char *hAlloc(int sz);
extern void hFree(char *data);
extern void hDump();

#endif // __HALLOC__