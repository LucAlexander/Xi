#include "bitmasks.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>

VECTOR_SOURCE(vu64_t, uint64_t)
VECTOR_SOURCE(vu32_t, uint32_t)

uint8_t maskCompare(vu64_t* reference, vu64_t* candidate){
	uint32_t i;
	if (reference->size > candidate->size){
		return 0;
	}
	uint64_t referenceChunk, candidateChunk;
	for (i = 0;i<reference->size;++i){
		referenceChunk = vu64_tGet(reference, i);
		candidateChunk = vu64_tGet(candidate, i);
		if ((referenceChunk & candidateChunk) != referenceChunk){
			return 0;
		}
	}
	return 1;
}

uint8_t maskEquals(vu64_t* a, vu64_t* b){
	uint32_t i;
	if (a->size != b->size){
		return 0;
	}
	uint64_t referenceChunk, candidateChunk;
	for (i = 0;i<a->size;++i){
		referenceChunk = vu64_tGet(a, i);
		candidateChunk = vu64_tGet(b, i);
		if (referenceChunk!=candidateChunk){
			return 0;
		}
	}
	return 1;
}

uint8_t maskContainsBit(vu64_t* mask, uint32_t bit){
	uint32_t chunk;
	uint32_t pos = bit;
	reduceMaskBitPair(&chunk, &pos);
	if (chunk >= mask->size){
		return 0;
	}
	uint64_t seg = vu64_tGet(mask, chunk);
	return (seg | (1<<pos)) == seg;
}

void maskRemoveBit(vu64_t* mask, uint32_t bit){
	uint32_t chunk;
	reduceMaskBitPair(&chunk, &bit);
	uint64_t* seg = vu64_tRef(mask, chunk);
	*seg &= ~(1<<bit);
}

void maskAddBit(vu64_t* mask, uint32_t bit){
	uint32_t chunk;
	reduceMaskBitPair(&chunk, &bit);
	while (chunk >= mask->size){
		vu64_tPushBack(mask, 0);
	}
	uint64_t* seg = vu64_tRef(mask, chunk);
	*seg |= (1<<bit);
}

void reduceMaskBitPair(uint32_t* index, uint32_t* pos){
	*index = (*pos)/64;
	*pos %= 64;
}

vu64_t createMask(uint32_t n, ...){
	vu32_t bits = vu32_tInit();
	vu32_tReserve(&bits, n);
	va_list v;
	va_start(v, n);
	uint32_t i;
	uint32_t maxBit = 0;
	for(i = 0;i<n;++i){
		uint32_t bit = va_arg(v, uint32_t);
		vu32_tPushBack(&bits, bit);
		if (bit > maxBit){
			maxBit = bit;
		}
	}
	va_end(v);
	maxBit /= 64;
	uint64_t* tempMask = calloc(maxBit+1, sizeof(uint64_t));
	for (i = 0;i<bits.size;++i){
		uint32_t pos = vu32_tGet(&bits, i);
		uint32_t index = 0;
		reduceMaskBitPair(&index, &pos);
		tempMask[index] |= 1<<pos;
	}
	vu64_t mask = vu64_tInit();
	for(i = 0;i<maxBit+1;++i){
		vu64_tPushBack(&mask, tempMask[i]);
	}
	vu32_tFree(&bits);
	free(tempMask);
	tempMask = NULL;
	return mask;
}

vu64_t maskCopy(vu64_t* mask){
	uint32_t i;
	vu64_t newMask = vu64_tInit();
	for (i = 0;i<mask->size;++i){
		vu64_tPushBack(&newMask, vu64_tGet(mask, i));
	}
	return newMask;
}

void maskChunkDisplay(uint64_t chunk){
	if (chunk == 0){
		printf(" 0 ");
	}
	while (chunk > 0){
		printf("%lu",(chunk%2));
		chunk /= 2;
	}
}

void maskDisplay(vu64_t* mask){
	uint32_t i;
	for (i = 0;i<mask->size;++i){
		uint64_t chunk = vu64_tGet(mask, i);
		maskChunkDisplay(chunk);
	}	
	printf("\n");
}
