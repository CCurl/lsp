#ifndef __HALLOC__

#define __HALLOC__

#include <stdio.h>
#include <stdint.h>

// Change these to control the heap and index sizes.
// Or, they can be set before #including this file.
// For details, see heap.c

#ifndef HEAPINDEX_SZ
#define HEAPINDEX_SZ 500
#endif

#ifndef HEAP_SZ
#define HEAP_SZ 10000
#endif

extern char *hAlloc(int sz);
extern void hFree(char *data);
void hDump(int details, FILE *toFP);

#endif // __HALLOC__
