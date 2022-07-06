#include "vector.h"
#include <stdio.h>

int8_t u8Compare(uint8_t a, uint8_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;
}

int8_t u16Compare(uint16_t a, uint16_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t u32Compare(uint32_t a, uint32_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t u64Compare(uint64_t a, uint64_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t i8Compare(int8_t a, uint8_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t i16Compare(int16_t a, uint16_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t i32Compare(int32_t a, uint32_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

int8_t i64Compare(int64_t a, uint64_t b){
	if (a<b){return -1;}
	if (a > b){return 1;}
	return 0;

}

Cvector CvectorInit(size_t size){
	uint32_t cap = 32;
	uint32_t s = 0;
	uint32_t ts = size;
	void* data = malloc(cap*ts);
	Cvector vec = {data, cap, s, ts};
	return vec;
}

CvectorIterator CvectorIteratorInit(Cvector* vec){
	CvectorIterator it = {vec, 0};
	return it;
}

uint8_t CvectorIteratorHasNext(CvectorIterator* it){
	return it->index < it->vec->size;
}

void* CvectorIteratorNext(CvectorIterator* it){
	return (uint8_t*)it->vec->data+(it->vec->typeSize*it->index++);
}

void CvectorFree(Cvector* vec){
	free(vec->data);
	vec->data = NULL;
	vec->size = 0;
}

void CvectorResize(Cvector* vec){
	void* temp = realloc(vec->data, vec->capacity*vec->typeSize*2);
	vec->data = temp;
	vec->capacity *= 2;
}

void CvectorPushBack(Cvector* vec, void* item){
	if (vec->size >= vec->capacity){
		CvectorResize(vec);
	}
	void* location = (uint8_t*)vec->data+(vec->typeSize*vec->size++);
	memcpy(location, item, vec->typeSize);
}

void CvectorInsert(Cvector* vec, uint32_t index, void* item){
	if (index >= vec->size){
		CvectorPushBack(vec, item);
		return;
	}
	if (vec->size >= vec->capacity){
		CvectorResize(vec);
	}
	vec->size++;
	uint32_t i;
	for (i = vec->size;i>0;--i){
		if (index == i){
			void* location = (uint8_t*)vec->data+(vec->typeSize*i);
			memcpy(location, item, vec->typeSize);
			return;
		}
		void* newSpace = (uint8_t*)vec->data+(i*vec->typeSize);
		void* oldSpace = (uint8_t*)vec->data+((i-1)*vec->typeSize);
		memcpy(newSpace, oldSpace, vec->typeSize);
	}
	memcpy(vec->data, item, vec->typeSize);
}

uint32_t CvectorPushInOrder(Cvector* vec, void* item, int8_t(*comparator)(void*, void*)){
	if (vec->size >= vec->capacity){
		CvectorResize(vec);
	}
	if (++vec->size==1){
		memcpy(vec->data, item, vec->typeSize);
		return 0;
	}
	uint32_t i;
	for (i = vec->size-1;i>0;--i){
		void* space = (uint8_t*)vec->data+((i-1)*vec->typeSize);
		if ((*comparator)(item, space) > 0){
			void* location = vec->data+(vec->typeSize*i);
			memcpy(location, item, vec->typeSize);
			return i;
		}
		void* newSpace = (uint8_t*)vec->data+(i*vec->typeSize);
		void* oldSpace = (uint8_t*)vec->data+((i-1)*vec->typeSize);
		memcpy(newSpace, oldSpace, vec->typeSize);
	}
	memcpy(vec->data, item, vec->typeSize);
	return 0;
}

void CvectorReserve(Cvector* vec, uint32_t places){
	if (vec->capacity-vec->size < places){
		uint32_t remaining = places - (vec->capacity - vec->size);
		void* temp = realloc(vec->data, (vec->capacity+remaining)*vec->typeSize);
		vec->data = temp;
		vec->capacity += remaining;
	}
}

void* CvectorGet(Cvector* vec, uint32_t index){
	if (index < vec->size){
		return (uint8_t*)vec->data+(index*vec->typeSize);
	}
	return NULL;
}

void* CvectorGetTrusted(Cvector* vec, uint32_t index){
	return (uint8_t*)vec->data+(index*vec->typeSize);
}

void CvectorSet(Cvector* vec, uint32_t index, void* item){
	if (index < vec->size){
		void* location = vec->data+(index*vec->typeSize);
		memcpy(location, item, vec->typeSize);
	}
}

void CvectorSetTrusted(Cvector* vec, uint32_t index, void* item){
	void* location = vec->data+(index*vec->typeSize);
	memcpy(location, item, vec->typeSize);
}

void CvectorShift(Cvector* vec, uint32_t index){
	uint32_t i;
	for (i = index;i<vec->size;++i){
		void* newSpace = (uint8_t*)vec->data+(i*vec->typeSize);
		void* oldSpace = (uint8_t*)vec->data+((i+1)*vec->typeSize);
		memcpy(newSpace, oldSpace, vec->typeSize);
	}
}

void CvectorRemoveInOrder(Cvector* vec, uint32_t index){
	if (index < vec->size){
		vec->size--;
		CvectorShift(vec, index);
	}
}

void CvectorRemove(Cvector* vec, uint32_t index){
	if (index < vec->size){
		void* res = (uint8_t*)vec->data+(index*vec->typeSize);
		void* shiftLoc = (uint8_t*)vec->data+(--vec->size*vec->typeSize);
		memcpy(res, shiftLoc, vec->typeSize);
	}
}

void* CvectorPop(Cvector* vec){
	return (uint8_t*)vec->data+(--vec->size*vec->typeSize);
}

void CvectorClear(Cvector* vec){
	vec->size = 0;
}


