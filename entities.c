#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "entities.h"
#include "cflags.h"
#include "mathutils.h"

#include "xi_components.h"
#include "systems.h"

VECTOR_SOURCE(su32_t, uint32_t)
HASHMAP_SOURCE(mu32u32_t, uint32_t, uint32_t, hashI)

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

uint8_t entity_active(entity_data* d, uint32_t eid){
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

uint8_t entity_exact_mask_logic(entity_data* d, uint32_t eid, uint8_t exact, uint64_t reference, uint64_t candidate){
	return entity_active(d, eid) && ((exact && (candidate==reference)) || (!(exact) && system_mask_compare(reference, candidate)));
}

uint8_t entity_exists_mask(entity_data* d, uint8_t exact, uint64_t mask){
	uint32_t i;
	for (i=0;i<d->entities;++i){
		uint64_t m = d->masks[i];
		if (entity_exact_mask_logic(d, i, exact, mask, m)){
			return 1;
		}
	}
	return 0;
}

uint8_t entity_exists(entity_data* d, uint8_t exact, uint32_t n, ...){
	uint64_t mask;
	va_list v;
	va_start(v, n);
	uint32_t i;
	for (i = 0;i<n;++i){
		mask = bit_on(mask, va_arg(v, uint32_t));
	}
	va_end(v);
	return entity_exists_mask(d, exact, mask);
}

uint32_t entity_nearest_mask(entity_data* d, v2 pos, uint8_t exact, uint64_t mask){
	int32_t distance = -1;
	uint32_t i, eid = 0;
	for (i=0;i<d->entities;++i){
		uint64_t m = d->masks[i];
		if (entity_exact_mask_logic(d, i, exact, mask, m)){
			v2* point = component_get(d, i, POSITION_C);
			uint32_t refdist = distancePointV2(pos, *point);
			if (distance == -1 || refdist < distance){
				eid = i;
				distance = refdist;
			}
		}
	}
	return eid;
}

uint32_t entity_nearest(entity_data* d, v2 pos, uint8_t exact, uint32_t n, ...){
	uint64_t mask;
	va_list v;
	va_start(v, n);
	uint32_t i;
	for (i = 0;i<n;++i){
		mask = bit_on(mask, va_arg(v, uint32_t));
	}
	va_end(v);
	return entity_nearest_mask(d, pos, exact, mask);
}

uint32_t entity_nearest_mask_n(entity_data* d, v2 pos, uint32_t n, uint8_t exact, uint64_t mask){
	mu32u32_t map = mu32u32_tInit();
	uint32_t i;
	for (i=0;i<d->entities;++i){
		uint64_t m = d->masks[i];
		if (entity_exact_mask_logic(d, i, exact, mask, m)){
			v2* point = component_get(d, i, POSITION_C);
			uint32_t refdist = distancePointV2(pos, *point);
			mu32u32_tPush(&map, refdist, i);
		}
	}
	uint32_t* keys = mu32u32_tGetKeySet(&map);
	uint32_t eid = mu32u32_tGet(&map, keys[n]).val;
	free(keys);
	keys = NULL;
	mu32u32_tFree(&map);
	return eid;
}

uint32_t entity_nearest_n(entity_data* d, v2 pos, uint8_t exact, uint32_t n, uint32_t m, ...){
	uint64_t mask;
	va_list v;
	va_start(v, m);
	uint32_t i;
	for (i = 0;i<m;++i){
		mask = bit_on(mask, va_arg(v, uint32_t));
	}
	va_end(v);
	return entity_nearest_mask_n(d, pos, n, exact, mask);
}

uint32_t entity_furthest_mask(entity_data* d, v2 pos, uint8_t exact, uint64_t mask){
	int32_t distance = -1;
	uint32_t i, eid = 0;
	for (i=0;i<d->entities;++i){
		uint64_t m = d->masks[i];
		if (entity_exact_mask_logic(d, i, exact, mask, m)){
			v2* point = component_get(d, i, POSITION_C);
			uint32_t refdist = distancePointV2(pos, *point);
			if (distance == -1 || refdist > distance){
				eid = i;
				distance = refdist;
			}
		}
	}
	return eid;
}

uint32_t entity_furthest(entity_data* d, v2 pos, uint8_t exact, uint32_t n, ...){
	uint64_t mask;
	va_list v;
	va_start(v, n);
	uint32_t i;
	for (i = 0;i<n;++i){
		mask = bit_on(mask, va_arg(v, uint32_t));
	}
	va_end(v);
	return entity_furthest_mask(d, pos, exact, mask);
}

uint32_t entity_furthest_mask_n(entity_data* d, v2 pos, uint32_t n, uint8_t exact, uint64_t mask){
	mu32u32_t map = mu32u32_tInit();
	uint32_t i;
	for (i=0;i<d->entities;++i){
		uint64_t m = d->masks[i];
		if (entity_exact_mask_logic(d, i, exact, mask, m)){
			v2* point = component_get(d, i, POSITION_C);
			uint32_t refdist = distancePointV2(pos, *point);
			mu32u32_tPush(&map, refdist, i);
		}
	}
	uint32_t* keys = mu32u32_tGetKeySet(&map);
	uint32_t eid = mu32u32_tGet(&map, keys[map.size-n]).val;
	free(keys);
	keys = NULL;
	mu32u32_tFree(&map);
	return eid;
}

uint32_t entity_furthest_n(entity_data* d, v2 pos, uint8_t exact, uint32_t n, uint32_t m, ...){
	uint64_t mask;
	va_list v;
	va_start(v, m);
	uint32_t i;
	for (i = 0;i<m;++i){
		mask = bit_on(mask, va_arg(v, uint32_t));
	}
	va_end(v);
	return entity_furthest_mask_n(d, pos, exact, n, mask);
}

