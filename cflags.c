#include "cflags.h"
#include <stdio.h>

uint64_t bit_flip(uint64_t flag, uint8_t bit){
	return flag ^ (1<<(bit-1));
}

uint64_t bit_off(uint64_t flag, uint8_t bit){
	return flag & ~(1<<(bit-1));
}

uint64_t bit_on(uint64_t flag, uint8_t bit){
	return flag | (1<<(bit-1));
}

uint8_t bit_check(uint64_t flag, uint8_t bit){
	return (flag & (1<<(bit-1)))!=0;
}

uint64_t bit_set(uint64_t flag, uint8_t pos, uint8_t bit){
	return ((flag & ~(1<<(pos-1))) | (bit << (pos-1)));
}

uint8_t bit_count(uint64_t flag){
	uint8_t count = 0;
	while (flag){
		flag &= (flag-1);
		count++;
	}
	return count;
}

void printflag64(uint64_t flag){
	int8_t i;
	for (i = FOB_64;i>0;--i){
		printf("%u",bit_check(flag, i));
	}
	printf("\n");
}

void printflag32(uint32_t flag){
	int8_t i;
	for (i = FOB_32;i>0;--i){
		printf("%u",bit_check(flag, i));
	}
	printf("\n");
}

void printflag16(uint16_t flag){
	int8_t i;
	for (i = FOB_16;i>0;--i){
		printf("%u",bit_check(flag, i));
	}
	printf("\n");
}

void printflag8(uint8_t flag){
	int8_t i;
	for (i = FOB_8;i>0;--i){
		printf("%u",bit_check(flag, i));
	}
	printf("\n");
}
