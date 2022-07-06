#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include <inttypes.h>
#include <stddef.h>
#include <string.h>

struct mem_arena;

typedef struct mem_arena{
	size_t capacity;
	size_t position;
	size_t resize_count;
	int32_t resize;
	uint8_t* pool;
	struct mem_arena* next;
}mem_arena;

mem_arena* mem_arena_alloc(size_t initial, size_t resize_size, uint32_t max_resizes);

void* mem_arena_request(mem_arena* arena, size_t request);

void mem_arena_realloc(mem_arena* arena);

void mem_arena_dealloc(mem_arena* arena);

void mem_arena_display(mem_arena* arena);

#define RESIZE_SENTINEL -7

#define REQUEST(type, name, arena)\
       	type* name = mem_arena_request(arena, sizeof(type)) 

#define REQUEST_SIZE(type, name, arena, size)\
	type* name = mem_arena_request(arena, size)

#define REQUEST_RESIZE(type, name, arena, size, newsize)\
	type* REALLOCATOR##_type##_name = mem_arena_request(arena, newsize);\
	memcpy(REALLOCATOR##_type##_name, name, sizeof(size));\
	name = REALLOCATOR##_type##_name
	

#endif
