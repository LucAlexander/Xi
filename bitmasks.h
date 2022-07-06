#ifndef BITMASKS_H
#define BITMASKS_H

#include "vector.h"
#include <inttypes.h>

VECTOR(vu64_t, uint64_t)
VECTOR(vu32_t, uint32_t)

uint8_t maskContainsBit(vu64_t* mask, uint32_t bit);
uint8_t maskCompare(vu64_t* reference, vu64_t* candidate);
uint8_t maskEquals(vu64_t* a, vu64_t* b);
void maskRemoveBit(vu64_t* mask, uint32_t bit);
void maskAddBit(vu64_t* mask, uint32_t bit);
void reduceMaskBitPair(uint32_t* index, uint32_t* pos);
vu64_t createMask(uint32_t n, ...);
vu64_t maskCopy(vu64_t* mask);
void maskDisplay(vu64_t* mask);
void maskChunkDisplay(uint64_t chunk);

#endif
