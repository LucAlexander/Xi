#ifndef TSV_H
#define TSV_H

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int8_t u8Compare(uint8_t a, uint8_t b);
int8_t u16Compare(uint16_t a, uint16_t b);
int8_t u32Compare(uint32_t a, uint32_t b);
int8_t u64Compare(uint64_t a, uint64_t b);

int8_t i8Compare(int8_t a, uint8_t b);
int8_t i16Compare(int16_t a, uint16_t b);
int8_t i32Compare(int32_t a, uint32_t b);
int8_t i64Compare(int64_t a, uint64_t b);

typedef struct Cvector{
	void* data;
	uint32_t capacity;
	uint32_t size;
	uint32_t typeSize;
}Cvector;

typedef struct CvectorIterator{
	Cvector* vec;
	uint32_t index;
}CvectorIterator;

Cvector CvectorInit(size_t size);
CvectorIterator CvectorIteratorInit(Cvector* vec);
uint8_t CvectorIteratorHasNext(CvectorIterator* it);
void* CvectorIteratorNext(CvectorIterator* it);
void CvectorFree(Cvector* vec);
void CvectorResize(Cvector* vec);
void CvectorPushBack(Cvector* vec, void* item);
void CvectorInsert(Cvector* vec, uint32_t index, void* item);
uint32_t CvectorPushInOrder(Cvector* vec, void* item, int8_t(*comparator)(void*, void*));
void CvectorReserve(Cvector* vec, uint32_t places);
void* CvectorGet(Cvector* vec, uint32_t index);
void* CvectorGetTrusted(Cvector* vec, uint32_t index);
void CvectorSet(Cvector* vec, uint32_t index, void* item);
void CvectorSetTrusted(Cvector* vec, uint32_t index, void* item);
void CvectorShift(Cvector* vec, uint32_t index);
void CvectorRemoveInOrder(Cvector* vec, uint32_t index);
void CvectorRemove(Cvector* vec, uint32_t index);
void* CvectorPop(Cvector* vec);
void CvectorClear(Cvector* vec);
 
#define VECTOR(typename, type) \
	typedef struct typename{ \
		type* data; \
		uint32_t capacity; \
		uint32_t size; \
	}typename; \
 \
	typedef struct typename##Iterator{ \
		typename* vec; \
		uint32_t index; \
	}typename##Iterator; \
 \
	typename typename##Init(); \
 \
	typename##Iterator typename##IteratorInit(typename* vec); \
 \
	uint8_t typename##IteratorHasNext(typename##Iterator* it); \
 \
	type typename##IteratorNext(typename##Iterator* it); \
 \
	void typename##Free(typename* vec); \
 \
	void typename##Resize(typename* vec); \
 \
	void typename##PushBack(typename* vec, type item); \
 \
 	void typename##Insert(typename* vec, uint32_t index, type item); \
 \
 	uint32_t typename##PushInOrder(typename* vec, type item, int8_t(*comparator)(type, type)); \
 \
	void typename##Reserve(typename* vec, uint32_t places); \
 \
	type typename##Get(typename* vec, uint32_t index); \
 \
	type typename##GetTruested(typename* vec, uint32_t index); \
 \
 	type* typename##Ref(typename* vec, uint32_t index); \
 \
 	type* typename##RefTrusted(typename* vec, uint32_t index); \
 \
 	void typename##Set(typename* vec, uint32_t index, type item); \
 \
 	void typename##SetTrusted(typename* vec, uint32_t index, type item); \
 \
	void typename##Shift(typename* vec, uint32_t index); \
 \
	type typename##RemoveInOrder(typename* vec, uint32_t index); \
 \
	type typename##Remove(typename* vec, uint32_t index); \
 \
	type typename##Pop(typename* vec); \
 \
	void typename##Clear(typename* vec);  \
 \


#define VECTOR_SOURCE(typename, type) \
	typename typename##Init(){ \
		uint32_t cap = 32; \
		uint32_t s = 0; \
		type* data = malloc(cap*sizeof(type)); \
		typename vec = {data, cap, s}; \
		return vec; \
	} \
 \
	typename##Iterator typename##IteratorInit(typename* vec){ \
		typename##Iterator it = {vec, 0}; \
		return it; \
	} \
 \
	uint8_t typename##IteratorHasNext(typename##Iterator* it){ \
		return it->index < it->vec->size; \
	} \
 \
	type typename##IteratorNext(typename##Iterator* it){ \
		return it->vec->data[it->index++]; \
	} \
 \
	void typename##Free(typename* vec){ \
		free(vec->data); \
		vec->data = NULL; \
		vec->size = 0; \
	} \
 \
	void typename##Resize(typename* vec){ \
		type* temp = realloc(vec->data, vec->capacity*sizeof(type)*2); \
		vec->data = temp; \
		vec->capacity*=2; \
	} \
 \
	void typename##PushBack(typename* vec, type item){ \
		if (vec->size >= vec->capacity){ \
			typename##Resize(vec); \
		} \
		vec->data[vec->size++] = item; \
	} \
 \
	void typename##Insert(typename* vec, uint32_t index, type item){ \
		if (index >= vec->size){ \
			typename##PushBack(vec, item); \
			return; \
		} \
		if (vec->size >= vec->capacity){ \
			typename##Resize(vec); \
		} \
		vec->size++; \
		uint32_t i; \
		for (i = vec->size;i>0;--i){ \
			if (index == i){ \
				vec->data[i] = item; \
				return; \
			} \
			vec->data[i] = vec->data[i-1]; \
		} \
		vec->data[0] = item; \
	} \
 \
 	uint32_t typename##PushInOrder(typename* vec, type item, int8_t(*comparator)(type, type)){ \
		if (vec->size >= vec->capacity){ \
			typename##Resize(vec); \
		} \
		if (++vec->size==1){ \
			vec->data[0] = item; \
			return 0; \
		} \
		uint32_t i; \
		for (i = vec->size-1;i>0;--i){ \
			if((*comparator)(item, vec->data[i-1]) > 0){ \
				vec->data[i] = item; \
				return i; \
			} \
			vec->data[i] = vec->data[i-1]; \
		} \
		vec->data[0] = item; \
		return 0; \
	} \
 \
	void typename##Reserve(typename* vec, uint32_t places){ \
		if (vec->capacity-vec->size < places){ \
			uint32_t remaining = places - (vec->capacity-vec->size); \
			type* temp = realloc(vec->data, (vec->capacity+remaining)*sizeof(type)); \
			vec->data = temp; \
			vec->capacity += remaining; \
		} \
	} \
 \
	type typename##Get(typename* vec, uint32_t index){ \
		type res; \
		if (index < vec->size){ \
			res = vec->data[index]; \
		} \
		return res; \
	} \
 \
 	type* typename##Ref(typename* vec, uint32_t index){ \
		if (index < vec->size){ \
			return &(vec->data[index]); \
		} \
		return NULL; \
	} \
 \
	type typename##GetTrusted(typename* vec, uint32_t index){ \
		return vec->data[index]; \
	} \
 \
 	type* typename##RefTrusted(typename* vec, uint32_t index){ \
		return &(vec->data[index]); \
	} \
 \
 	void typename##Set(typename* vec, uint32_t index, type item){ \
		if (index < vec->size){ \
			vec->data[index] = item; \
		} \
	} \
 \
	void typename##SetTrusted(typename* vec, uint32_t index, type item){ \
		vec->data[index] = item; \
	} \
 \
	void typename##Shift(typename* vec, uint32_t index){ \
		uint32_t i; \
		for (i = index;i<vec->size;++i){ \
			vec->data[i] = vec->data[i+1]; \
		} \
	} \
 \
	type typename##RemoveInOrder(typename* vec, uint32_t index){ \
		type res; \
		if (index < vec->size){ \
			res = vec->data[index]; \
			vec->size--; \
			typename##Shift(vec, index); \
		} \
		return res; \
	} \
\
	type typename##Remove(typename* vec, uint32_t index){ \
		type res; \
		if (index < vec->size){ \
			res = vec->data[index]; \
			vec->data[index] = vec->data[--vec->size]; \
		} \
		return res; \
	} \
 \
	type typename##Pop(typename* vec){ \
		return vec->data[--vec->size]; \
	} \
 \
	void typename##Clear(typename* vec){  \
		vec->size = 0; \
	} \
 \

#endif

