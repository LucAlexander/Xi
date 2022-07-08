#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "bitmasks.h"
#include "hashMap.h"
#include <stdarg.h>

#define SYSTEM_ARG_REQUIREMENTS struct xi_utils* xi, uint32_t id, struct vec_t* entity, struct mu32_u32* relation

#define SYSTEM(name) void name(SYSTEM_ARG_REQUIREMENTS)
#define ARG(variable, component) variable = vec_tGet(entity, mu32_u32Get(relation, component).val)

struct entity_data;
struct vec_t;
struct xi_utils;
struct archetype_v2;
struct mu32_u32;

typedef struct system_t{
	vu64_t mask;
	uint64_t filter;
	uint64_t magnet;
	void (*func)(SYSTEM_ARG_REQUIREMENTS);
}system_t;

system_t system_init(void f(SYSTEM_ARG_REQUIREMENTS), uint32_t n, ...);
void system_add_filter(system_t* sys, uint32_t n, ...);
void system_remove_filter(system_t* sys, uint32_t n, ...);
void system_add_magnet(system_t* sys, uint32_t n, ...);
void system_remove_magnet(system_t* sys, uint32_t n, ...);
void system_run(system_t* sys, struct xi_utils* , int32_t layer);
void system_run_archetype(system_t* sys, struct archetype_v2*, struct xi_utils*, int32_t layer);
void system_free(system_t* sys);
uint8_t system_filter(system_t* sys, uint64_t entity_mask);
uint8_t system_layer_check(int32_t layer, struct entity_data* medium, uint32_t eid);

#endif
