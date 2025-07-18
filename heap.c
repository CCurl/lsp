/*---------------------------------------------------------------------------*/
/* A simple heap allocator */

#include "heap.h"

#define BTWI(n,l,h) (((l) <= (n)) && ((n) <= (h)))

typedef unsigned int uint;
typedef struct {
	uint inUse: 1, sz: 31, off;
} HEAP_T, *PHEAP;

static uint32_t hHere = 0, iHere = 0;
static HEAP_T index[HEAPINDEX_SZ];
static char heap[HEAP_SZ];
static const int hASG = 8; // alloc size granularity

void hDump(int details, FILE *toFP) {
	fprintf(toFP ? toFP : stdout, "heap component information:\n");
	fprintf(toFP ? toFP : stdout, "----------------------------------------\n");
	fprintf(toFP ? toFP : stdout, "heap  - size: %u bytes, %u used\n", HEAP_SZ, hHere);
	fprintf(toFP ? toFP : stdout, "index - size: %u records, %u used\n", HEAPINDEX_SZ, iHere);
	fprintf(toFP ? toFP : stdout, "other - index struct sz: %u, granularity, %d bytes\n", sizeof(HEAP_T), hASG);
	if (details) {
		for (uint i = 0; i < iHere; i++) {
			PHEAP x = (PHEAP)&index[i];
			fprintf(toFP ? toFP : stdout, "%3d, inuse: %u, sz: %2u, off: %u\n",
				i, x->inUse, x->sz, x->off);
		}
	}
}

static int hFindFree(uint sz) {
	uint best = -1;
	for (uint i = 0; i < iHere; i++) {
		PHEAP x = &index[i];
		if ((x->inUse == 0) && (sz <= x->sz)) {
			if (x->sz == sz) { return i; }
			if ((best == -1) || (index[best].sz < x->sz)) { best = i; }
		}
	}
	return best;
}

char *hAlloc(int sz) {
	if (sz == 0) { sz = 1; }
	if ((sz % hASG) != 0) { sz += hASG - (sz % hASG); }

	int hi = hFindFree(sz);
	if (0 <= hi) {
		index[hi].inUse = 1;
		return &heap[index[hi].off];
	}

	if (HEAP_SZ <= (hHere + sz)) { return NULL; }
	if (HEAPINDEX_SZ <= iHere) { return NULL; }

	PHEAP x = &index[iHere++];
	x->sz = sz;
	x->off = hHere;
	x->inUse = 1;
	hHere += sz;
	return &heap[x->off];
}

static int hFindIndex(char *data) {
	int32_t off = data - &heap[0];
	if (!BTWI(off, 0, HEAP_SZ - 1)) { return -1; }
	for (uint i = 0; i < iHere; i++) {
		if (index[i].off == off) { return i; }
	}
	return -1;
}

void hFree(char *data) {
	int hi = hFindIndex(data);
	if (hi == -1) { return; }
	PHEAP x = &index[hi];
	x->inUse = 0;
	while (0 < iHere) {
		x = (PHEAP)&index[iHere - 1];
		if (x->inUse) { break; }
		hHere = x->off;
		x->off = x->sz = 0;
		iHere--;
	}
}
