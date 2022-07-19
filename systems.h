#ifndef SYSTEM_H
#define SYSTEM_H

#include <inttypes.h>

#include "entities.h"
#include "xi.h"

#define SYSTEM_ARG_REQUIREMENTS struct xi_utils* xi, uint32_t id 
#define SYSTEM_ARGS xi, id

#define SYSTEM(name) void name(SYSTEM_ARG_REQUIREMENTS)
#define ARG(variable, component) variable = component_get(xi->ecs, id, component)

typedef struct system_t{
	uint64_t mask;
	// T&M | T&~F | ~R&~T
	uint64_t filter;
	uint64_t requisite;
	uint64_t magnet;
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
}system_t;

system_t system_init(void f(SYSTEM_ARG_REQUIREMENTS), uint32_t n, ...);
void system_add_requirement(system_t* sys, uint32_t n, ...);
void system_remove_requirement(system_t* sys, uint32_t n, ...);
void system_add_filter(system_t* sys, uint32_t n, ...);
void system_remove_filter(system_t* sys, uint32_t n, ...);
void system_add_magnet(system_t* sys, uint32_t n, ...);
void system_remove_magnet(system_t* sys, uint32_t n, ...);
uint8_t system_filter(system_t sys, uint64_t targetFlag);
uint8_t system_mask_compare(uint64_t reference, uint64_t candidate);
void system_run(system_t s, xi_utils* xi, uint16_t layer);

#endif
