#include "systems.h"
#include "entities.h"
#include "xi.h"
#include "cflags.h"

#include <stdarg.h>

system_t system_init(void f(SYSTEM_ARG_REQUIREMENTS), uint32_t n, ...){
	system_t s;
	s.f = f;
	s.filter = ENTITY_DEACTIVATED;
	s.magnet = 0;
	s.requisite = 0;
	va_list v;
	va_start(v, n);
	uint32_t i;
	s.mask = 0;
	for (i = 0;i<n;++i){
		uint32_t cid = va_arg(v, uint32_t);
		s.mask = bit_on(s.mask, cid);
	}
	va_end(v);
	return s;
}

void system_add_requirement(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->requisite = bit_on(sys->requisite, va_arg(v, uint32_t));
	}
	va_end(v);

}

void system_remove_requirement(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->requisite = bit_off(sys->requisite, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_add_filter(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->filter = bit_on(sys->filter, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_remove_filter(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->filter = bit_off(sys->filter, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_add_magnet(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->magnet = bit_on(sys->magnet, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_remove_magnet(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->magnet = bit_off(sys->magnet, va_arg(v, uint32_t));
	}
	va_end(v);
}

uint8_t system_filter(system_t sys, uint64_t targetFlag){
	return (
		(targetFlag & sys.magnet) |
		(targetFlag & (~sys.filter)) |
		((~targetFlag) & (~sys.requisite))
	) == -1;
}

uint8_t system_mask_compare(uint64_t reference, uint64_t candidate){
	return (reference & candidate) == reference;
}

void system_run(system_t s, xi_utils* xi, uint16_t layer){
	uint32_t id;
	uint32_t n = xi->ecs->entities;
	for (id = 0;id<n;++id){
		if (
		system_mask_compare(s.mask, xi->ecs->masks[id]) &&
		system_filter(s, xi->ecs->flags[id]) &&
		((layer == 0) || (layer == xi->ecs->layers[id]))
		){
			s.f(SYSTEM_ARGS);
		}
	}
}
