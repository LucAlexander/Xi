#ifndef MINPQ_H
#define MINPQ_H

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>

#define MIN_PQ_STARTING_CAPACITY 32

uint32_t mpq_root(uint32_t i);
uint32_t mpq_left(uint32_t i);
uint32_t mpq_right(uint32_t i);

#define MIN_PQ(typename, type)\
	typedef struct typename##_minpq_node_t{\
		uint32_t order;\
		type data;\
	}typename##_minpq_node_t;\
\
	typedef struct typename##_t{\
		typename##_minpq_node_t* data;\
		uint32_t size;\
		uint32_t capacity;\
	}typename##_t;\
\
	typename##_t typename##_init();\
	void typename##_free(typename##_t* mpq);\
	void typename##_insert(typename##_t* mpq, uint32_t order, type data);\
	type typename##_pop(typename##_t* mpq);\
	uint32_t typename##_min(typename##_t* mpq);\
\
	void typename##_shift_up(typename##_t* mpq, uint32_t i);\
	void typename##_shift_down(typename##_t* mpq, uint32_t i);\
\


#define MIN_PQ_SOURCE(typename, type)\
	typename##_t typename##_init(){\
		typename##_t mpq;\
		mpq.size = 0;\
		mpq.capacity = MIN_PQ_STARTING_CAPACITY;\
		mpq.data = malloc(mpq.capacity*sizeof(typename##_minpq_node_t));\
		return mpq;\
	}\
\
	void typename##_free(typename##_t* mpq){\
		free(mpq->data);\
		mpq->data = NULL;\
	}\
\
	void typename##_insert(typename##_t* mpq, uint32_t order, type data){\
		if (mpq->size == mpq->capacity){\
			void* new_buffer = realloc(mpq->data, mpq->capacity*sizeof(typename##_minpq_node_t)*2);\
			mpq->data = new_buffer;\
			mpq->capacity *= 2;\
		}\
		typename##_minpq_node_t item = {order, data};\
		mpq->data[mpq->size] = item;\
		typename##_shift_up(mpq, mpq->size++);\
	}\
\
	type typename##_pop(typename##_t* mpq){\
		type res = mpq->data[0].data;\
		mpq->data[0] = mpq->data[--mpq->size];\
		typename##_shift_down(mpq, 0);\
		return res;\
	}\
\
	uint32_t typename##_min(typename##_t* mpq){\
		return mpq->data[0].order;\
	}\
\
	void typename##_shift_up(typename##_t* mpq, uint32_t i){\
		while (i > 0 && mpq->data[mpq_root(i)].order > mpq->data[i].order){\
			typename##_minpq_node_t temp = mpq->data[mpq_root(i)];\
			mpq->data[mpq_root(i)] = mpq->data[i];\
			mpq->data[i] = temp;\
			i = mpq_root(i);\
		}\
	}\
\
	void typename##_shift_down(typename##_t* mpq, uint32_t i){\
		uint32_t peak = i;\
		uint32_t l = mpq_left(i);\
		uint32_t r = mpq_right(i);\
		if (l<=mpq->size && mpq->data[l].order < mpq->data[peak].order){\
			peak = l;\
		}\
		if (r<=mpq->size && mpq->data[r].order < mpq->data[peak].order){\
			peak = r;\
		}\
		if (i != peak){\
			typename##_minpq_node_t temp = mpq->data[i];\
			mpq->data[i] = mpq->data[peak];\
			mpq->data[peak] = temp;\
			typename##_shift_down(mpq, peak);\
		}\
	}\
\

#endif
