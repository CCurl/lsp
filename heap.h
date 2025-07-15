#ifndef __HALLOC__

#define __HALLOC__

#include <stdio.h>
#include <stdint.h>

// Change these to control heap size and index
// NOTE: MAX index size is 32768 entries
//       MAX alloc sz is 65536
// For details, see HEAP_T in heap.c

#define HEAPINDEX_SZ 500
#define HEAP_SZ 10000

extern char *hAlloc(int sz);
extern void hFree(char *data);
extern void hDump();

#endif // __HALLOC__
