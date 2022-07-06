#include "hashMap.h"

uint8_t hashI(uint64_t i, uint32_t capacity){
	return i % capacity;
}

uint8_t hashS(const char* key, uint32_t capacity){
	uint32_t result = 0;
	uint32_t len = strlen(key);
	uint32_t i;
	for (i = 0;i<len;++i){
		result += key[i] * pow(128, len-1-i);
	}
	return result%capacity;
}
