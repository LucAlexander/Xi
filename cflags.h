#ifndef C_FLAGS_H
#define C_FLAGS_H

#include <stdint.h>

#define FOB_64 64
#define FOB_32 32
#define FOB_16 16
#define FOB_8 8

uint64_t bit_flip(uint64_t flag, uint8_t bit);
uint64_t bit_off(uint64_t flag, uint8_t bit);
uint64_t bit_on(uint64_t flag, uint8_t bit);
uint8_t bit_check(uint64_t flag, uint8_t bit);
uint64_t bit_set(uint64_t flag, uint8_t pos, uint8_t bit);

uint8_t bit_count(uint64_t flag);

void printflag64(uint64_t flag);
void printflag32(uint32_t flag);
void printflag16(uint16_t flag);
void printflag8(uint8_t flag);

#endif

