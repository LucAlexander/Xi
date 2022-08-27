#include "minpq.h"

uint32_t mpq_root(uint32_t i){
	return (i-1)/2;
}

uint32_t mpq_left(uint32_t i){
	return (2*i)+1;
}

uint32_t mpq_right(uint32_t i){
	return (2*i)+2;
}


