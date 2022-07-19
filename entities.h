#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"

#define ENTITY_LIMIT 65536
#define COMPONENT_LIMIT 64

VECTOR(su32_t, uint32_t)

typedef enum FLAGS{
	ENTITY_DEACTIVATED=1
}FLAGS;

typedef struct entity_data{
	void* data;
	uint32_t entities;
	su32_t stack;
	uint64_t* masks;
	uint64_t* flags;
	uint16_t* layers;
	uint32_t sizes[COMPONENT_LIMIT];
	uint32_t offsets[COMPONENT_LIMIT];
	uint32_t size;
}entity_data;

void ecs_init(entity_data* d, uint32_t n, ...);
void* component_get(entity_data* d, uint32_t eid, uint32_t cid);
void component_add(entity_data* d, uint32_t eid, uint32_t cid, void* data);
void component_remove(entity_data* d, uint32_t eid, uint32_t cid);
uint32_t entity_create(entity_data* d);
void entity_destroy(entity_data* d, uint32_t eid);
void entity_set_layer(entity_data* d, uint32_t eid, uint16_t layer);
uint16_t entity_max_layer(entity_data* d);
uint16_t entity_get_layer(entity_data* d, uint32_t eid);
uint8_t entity_exists(entity_data* d, uint32_t eid);
void ecs_clear(entity_data* d);
void ecs_deinit(entity_data* d);

#endif
