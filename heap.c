/*---------------------------------------------------------------------------*/
/* A simple heap allocator */

#include "heap.h"

#define BTWI(n,l,h) (((l) <= (n)) && ((n) <= (h)))

typedef unsigned int uint;
typedef struct {
	uint32_t sz, inUse, off;
} HEAP_T, *PHEAP;

uint32_t hHere = 0, hiCount = 0;
HEAP_T hIndex[HEAPINDEX_SZ];
char heap[HEAP_SZ];

void hDump() {
	printf("\nn-%u/h-%u", hiCount, hHere);
	for (uint i = 0; i < hiCount; i++) {
		PHEAP x = (PHEAP)&hIndex[i];
		printf("\nhi-%d/iu-%u/sz-%u/off-%u/d:[%s]",
			i, x->inUse, x->sz, x->off, &heap[x->off]);
	}
	// printf("\n");
}

static int hFindFree(uint sz) {
	uint best = -1;
	for (uint i = 0; i < hiCount; i++) {
		PHEAP x = &hIndex[i];
		if ((x->inUse == 0) && (sz <= x->sz)) {
			if (x->sz == sz) { return i; }
			if ((best == -1) || (hIndex[best].sz < x->sz)) { best = i; }
		}
	}
	return best;
}

char *hAlloc(int sz) {
	const int hASG = 8; // alloc size granularity
	if (sz == 0) { sz = 1; }
	if ((sz % hASG) != 0) { sz += hASG - (sz % hASG); }

	int hi = hFindFree(sz);
	if (0 <= hi) {
		hIndex[hi].inUse = 1;
		return &heap[hIndex[hi].off];
	}

	if (HEAP_SZ <= (hHere + sz)) { return NULL; }
	if (HEAPINDEX_SZ <= hiCount) { return NULL; }

	PHEAP x = &hIndex[hiCount++];
	x->sz = sz;
	x->off = hHere;
	x->inUse = 1;
	hHere += sz;
	return &heap[x->off];
}

static int hFindData(char *data) {
	int32_t off = data - &heap[0];
	if (!BTWI(off, 0, HEAP_SZ - 1)) { return -1; }
	for (uint i = 0; i < hiCount; i++) {
		if (hIndex[i].off == off) { return i; }
	}
	return -1;
}

void hFree(char *data) {
	int hi = hFindData(data);
	if (hi == -1) { return; }
	PHEAP x = &hIndex[hi];
	x->inUse = 0;
	while (0 < hiCount) {
		x = (PHEAP)&hIndex[hiCount - 1];
		if (x->inUse) { break; }
		hHere = x->off;
		x->off = x->sz = 0;
		hiCount--;
	}
}
