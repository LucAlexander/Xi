#ifndef TSHM_H
#define TSHM_H

#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <stdio.h>

uint8_t hashI(uint64_t key, uint32_t capacity);
uint8_t hashS(const char* key, uint32_t capacity);

#define HASHMAP_CAPACITY 31

#define HASHMAP(typename, keyType, valType) \
	typedef struct typename##TSHM_NODE{ \
		struct typename##TSHM_NODE* next; \
		keyType key; \
		valType val; \
	}typename##TSHM_NODE; \
	\
	typedef struct typename{ \
		typename##TSHM_NODE** data; \
		uint32_t capacity; \
		uint32_t size; \
	}typename; \
	\
	typedef struct typename##Result{ \
		uint8_t error; \
		keyType key; \
		valType val; \
	}typename##Result; \
	\
	typedef struct typename##Iterator{ \
		typename* map; \
		typename##TSHM_NODE* current; \
		int32_t index; \
	}typename##Iterator; \
	\
	typename typename##Init(); \
	\
	typename##TSHM_NODE* typename##TSHM_NODE_INIT(keyType key, valType val); \
	\
	typename##Iterator typename##IteratorInit(typename* map); \
	\
	typename##Result typename##IteratorNext(typename##Iterator* it); \
	\
	uint8_t typename##IteratorHasNext(typename##Iterator* it); \
	\
	void typename##TSHM_NODE_FREE(typename##TSHM_NODE* n); \
	\
	void typename##Free(typename* map); \
	\
	uint8_t typename##TSHM_NODE_INSERT(typename##TSHM_NODE* n, keyType key, valType val); \
	\
	keyType* typename##GetKeySet(typename* map); \
	\
	void typename##Push(typename* map, keyType key, valType val); \
	\
	typename##Result typename##Get(typename* map, keyType key); \
	\
	valType* typename##Ref(typename* map, keyType key); \
	\
	uint8_t typename##Contains(typename* map, keyType key); \
	\
	typename##Result typename##Pop(typename* map, keyType key); \
	\
	void typename##Clear(typename* map); \
	\


#define HASHMAP_SOURCE(typename, keyType, valType, hashing) \
	typename typename##Init(){ \
		uint32_t cap = HASHMAP_CAPACITY; \
		typename##TSHM_NODE** d = calloc(cap, sizeof(typename##TSHM_NODE*)); \
		typename map = {d, cap, 0}; \
		return map; \
	} \
	\
	typename##TSHM_NODE* typename##TSHM_NODE_INIT(keyType key, valType val){ \
		typename##TSHM_NODE* n = malloc(sizeof(typename##TSHM_NODE)); \
		n->key = key; \
		n->val = val; \
		n->next = NULL; \
		return n; \
	} \
	\
	typename##Iterator typename##IteratorInit(typename* map){ \
		uint32_t i; \
		for (i = 0;i<map->capacity&&map->data[i]==NULL;++i){} \
		if (i==map->capacity){ \
			typename##TSHM_NODE* current = NULL; \
			typename##Iterator it = {map, current, -1}; \
			return it; \
		} \
		typename##TSHM_NODE* current = map->data[i]; \
		typename##Iterator it = {map, current, i}; \
		return it; \
	} \
	\
	typename##Result typename##IteratorNext(typename##Iterator* it){ \
		typename##TSHM_NODE* temp = it->current; \
		if (!temp||it->index==-1){ \
			typename##Result res = {.error = 1}; \
			return res; \
		} \
		it->current = it->current->next; \
		if (it->current!=NULL){ \
			typename##Result res = {0, temp->key, temp->val}; \
			return res; \
		} \
		uint32_t i; \
		for (i = ++it->index;i<it->map->capacity&&it->map->data[i]==NULL;++i){} \
		if (i==it->map->capacity){ \
			it->index = -1; \
			typename##Result res = {0, temp->key, temp->val}; \
			return res; \
		} \
		it->current = it->map->data[i]; \
		it->index = i; \
		typename##Result res = {0, temp->key, temp->val}; \
		return res; \
	} \
	\
	uint8_t typename##IteratorHasNext(typename##Iterator* it){ \
		return (it!=NULL) && (it->current!=NULL) && (it->index != -1); \
	} \
	\
	void typename##TSHM_NODE_FREE(typename##TSHM_NODE* n){ \
		while(n){ \
			typename##TSHM_NODE* temp = n; \
			n = n->next; \
			free(temp); \
			temp = NULL; \
		} \
		n = NULL; \
	} \
	\
	void typename##Free(typename* map){ \
		uint32_t i; \
		map->size = 0; \
		for (i = 0;i<map->capacity;++i){ \
			if (map->data[i]){ \
				typename##TSHM_NODE_FREE(map->data[i]); \
			} \
		} \
		free(map->data); \
		map->data = NULL; \
	} \
	\
	uint8_t typename##TSHM_NODE_INSERT(typename##TSHM_NODE* n, keyType key, valType val){ \
		typename##TSHM_NODE* last = n; \
		while (n){ \
			if (n->key==key){ \
				n->val = val; \
				return 1; \
			} \
			last = n; \
			n = n->next; \
		} \
		last->next = typename##TSHM_NODE_INIT(key, val); \
		return 0; \
	} \
	\
	keyType* typename##GetKeySet(typename* map){ \
		typename##Iterator it = typename##IteratorInit(map); \
		keyType* set = malloc(map->size*sizeof(keyType)); \
		uint32_t i = 0; \
		while (typename##IteratorHasNext(&it)){ \
			set[i++] = typename##IteratorNext(&it).key; \
		} \
		return set; \
	} \
	\
	void typename##Push(typename* map, keyType key, valType val){ \
		uint32_t index = hashing(key, map->capacity); \
		if (map->data[index]){ \
			uint8_t replace = typename##TSHM_NODE_INSERT(map->data[index], key, val); \
			if (replace == 0){ \
				map->size++; \
			} \
			return; \
		} \
		map->size++; \
		map->data[index] = typename##TSHM_NODE_INIT(key, val); \
	} \
	\
	typename##Result typename##Get(typename* map, keyType key){ \
		if (!map || map->capacity==0){ \
			typename##Result res = {.error=2}; \
			return res; \
		} \
		uint32_t index = hashing(key, map->capacity); \
		typename##TSHM_NODE* temp; \
		temp = map->data[index]; \
		while(temp!=NULL){ \
			if (temp->key == key){ \
				typename##Result res = {0, temp->key, temp->val}; \
				return res; \
			} \
			temp = temp->next; \
		} \
		typename##Result res = {.error = 1}; \
		return res; \
	} \
	\
	valType* typename##Ref(typename* map, keyType key){ \
		if (!map || map->capacity==0){ \
			return NULL; \
		} \
		uint32_t index = hashing(key, map->capacity); \
		typename##TSHM_NODE* temp; \
		temp = map->data[index]; \
		while(temp!=NULL){ \
			if (temp->key == key){ \
				return &(temp->val); \
			} \
			temp = temp->next; \
		} \
		return NULL; \
	} \
	\
	uint8_t typename##Contains(typename* map, keyType key){ \
		return typename##Ref(map, key) != NULL; \
	} \
	\
	typename##Result typename##Pop(typename* map, keyType key){ \
		if (!map||map->size == 0){ \
			typename##Result res = {.error=2}; \
			return res; \
		} \
		uint32_t index = hashing(key, map->capacity); \
		typename##TSHM_NODE* temp = map->data[index]; \
		typename##TSHM_NODE* last = NULL; \
		while (temp){ \
			if (temp->key == key){ \
				if (!last){ \
					map->data[index] = temp->next; \
					temp->next = NULL; \
					keyType ky = temp->key; \
					valType vl = temp->val; \
					typename##TSHM_NODE_FREE(temp); \
					typename##Result res = {0,ky,vl}; \
					map->size--; \
					return res; \
				} \
				last->next = temp->next; \
				temp->next = NULL; \
				valType vl = temp->val; \
				keyType ky = temp->key; \
				typename##TSHM_NODE_FREE(temp); \
				typename##Result res = {0, ky, vl}; \
				map->size--; \
				return res; \
			} \
			last = temp; \
			temp = temp->next; \
		} \
		typename##Result res = {.error=1}; \
		return res; \
	} \
	\
	void typename##Clear(typename* map){ \
		uint32_t i; \
		for (i = 0;i<map->capacity;++i){ \
			typename##TSHM_NODE_FREE(map->data[i]); \
			map->data[i] = NULL; \
		} \
		map->size = 0; \
	} \
	\

#endif

