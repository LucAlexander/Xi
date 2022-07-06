#include "memarena.h"

#include <stdlib.h>

#include <stdio.h>

mem_arena* mem_arena_alloc(size_t initial, size_t resize_size, uint32_t max_resizes){
	mem_arena* arena = malloc(sizeof(mem_arena));
	arena->capacity = initial;
	arena->resize = resize_size;
	arena->resize_count = max_resizes;
	arena->position = 0;
	arena->pool = calloc(initial, sizeof(uint8_t));
	arena->next = NULL;
	return arena;
}

void* mem_arena_request(mem_arena* arena, size_t request){
	if (arena->position + request < arena->capacity){
		uint8_t* byte = (uint8_t*)arena->pool + arena->position*sizeof(uint8_t);
		arena->position += request;
		return byte;
	}
	if (arena->next != NULL){
		return mem_arena_request(arena->next, request);
	}
	if (arena->resize_count > 0){
		mem_arena_realloc(arena);
		return mem_arena_request(arena->next, request);
	}
	return NULL;
}

void mem_arena_realloc(mem_arena* arena){
	arena->next = mem_arena_alloc(arena->resize, arena->resize, arena->resize_count-1);
	arena->resize_count = RESIZE_SENTINEL;
}

void mem_arena_dealloc(mem_arena* arena){
	free(arena->pool);
	arena->pool = NULL;
	if (arena->next != NULL){
		mem_arena_dealloc(arena->next);
	}
	free(arena);
	arena = NULL;
}

void mem_arena_display(mem_arena* arena){
	printf("[arena:%lu/%lu]",arena->position,arena->capacity);
	if (arena->next != NULL){
		printf("->");
		mem_arena_display(arena->next);
		return;
	}
	printf("\n");
}
