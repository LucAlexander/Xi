#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "entities.h"
#include "cflags.h"

VECTOR_SOURCE(su32_t, uint32_t)

void ecs_init(entity_data* d, uint32_t n, ...){
	d->entities = 0;
	d->size = 0;
	d->stack = su32_tInit();
	va_list v;
	va_start(v, n);
	uint32_t i, offset = 0;
	for(i = 0;i<n;++i){
		uint32_t s = va_arg(v, uint32_t);
		d->sizes[i] = s;
		d->offsets[i] = offset;
		d->size += s;
		offset += s;
	}
	va_end(v);
	d->data = malloc(ENTITY_LIMIT * d->size);
	d->masks = (uint64_t*)calloc(ENTITY_LIMIT, sizeof(uint64_t));
	d->flags = (uint64_t*)calloc(ENTITY_LIMIT, sizeof(uint64_t));
	d->layers = (uint16_t*)calloc(ENTITY_LIMIT, sizeof(uint16_t));
}

void* component_get(entity_data* d, uint32_t eid, uint32_t cid){
	return ((uint8_t*)d->data) + (d->offsets[cid]*ENTITY_LIMIT)+(d->sizes[cid]*eid);
}

void component_add(entity_data* d, uint32_t eid, uint32_t cid, void* data){
	void* pos = component_get(d, eid, cid);
	memcpy(pos, data, d->sizes[cid]);
	d->masks[eid] = bit_on(d->masks[eid], cid);
}

void component_remove(entity_data* d, uint32_t eid, uint32_t cid){
	d->masks[eid] = bit_off(d->masks[eid], cid);
}

uint32_t entity_create(entity_data* d){
	uint32_t eid = (d->stack.size != 0) ? su32_tPop(&d->stack) : d->entities++;
	d->flags[eid] = 0;
	return eid;
}

void entity_set_layer(entity_data* d, uint32_t eid, uint16_t layer){
	d->layers[eid] = layer;
}

uint16_t entity_get_layer(entity_data* d, uint32_t eid){
	return d->layers[eid];
}

uint16_t entity_max_layer(entity_data* d){
	uint32_t i;
	uint16_t m = 0;
	for (i = 0;i<d->entities;++i){
		uint16_t layer = d->layers[i];
		if (m < layer){
			m = layer;
		}
	}
	return m;
}

uint8_t entity_exists(entity_data* d, uint32_t eid){
	return !bit_check(d->flags[eid], ENTITY_DEACTIVATED);
}

void entity_destroy(entity_data* d, uint32_t eid){
	d->flags[eid] = bit_on(d->flags[eid], ENTITY_DEACTIVATED);
	d->masks[eid] = 0;
	d->layers[eid] = 0;
	su32_tPushBack(&d->stack, eid);
}

void ecs_clear(entity_data* d){
	uint32_t i;
	for (i = 0;i<d->entities;++i){
		entity_destroy(d, i);
	}
}

void ecs_deinit(entity_data* d){
	su32_tFree(&d->stack);
	free(d->masks);
	free(d->data);
	free(d->flags);
	free(d->layers);
	d->masks = NULL;
	d->data = NULL;
	d->flags = NULL;
	d->layers = NULL;
}

