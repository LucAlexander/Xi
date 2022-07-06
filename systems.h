#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "bitmasks.h"
#include <stdarg.h>

struct entity_data;
struct vec_t;

typedef struct system_t{
	vu64_t mask;
	uint64_t filter;
	uint64_t magnet;
	void (*func)(struct entity_data*, uint32_t, struct vec_t*);
}system_t;

system_t system_init(void f(struct entity_data*, uint32_t, struct vec_t*), uint32_t n, ...);
void system_add_filter(system_t* sys, uint32_t n, ...);
void system_remove_filter(system_t* sys, uint32_t n, ...);
void system_add_magnet(system_t* sys, uint32_t n, ...);
void system_remove_magnet(system_t* sys, uint32_t n, ...);
void system_run(system_t* sys, struct entity_data* medium, uint32_t layer);
void system_free(system_t* sys);
uint8_t system_filter(system_t* sys, uint64_t entity_mask);
uint8_t system_layer_check(uint32_t layer, struct entity_data* medium, uint32_t eid);

#endif
