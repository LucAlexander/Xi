#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "hashMap.h"

#define ENTITY_LIMIT 1024
#define COMPONENT_LIMIT 128

VECTOR(su32_t, uint32_t)
VECTOR(vf32_t, float)
HASHMAP(mu32u32_t, uint32_t, uint32_t)

struct v2;

typedef enum FLAGS{
	ENTITY_DEACTIVATED=1,
	ENTITY_SOLID=2
}FLAGS;

typedef struct entity_data{
	void* data;
	uint32_t entities;
	su32_t stack;
	vf32_t signals;
	uint64_t* masks;
	uint64_t* flags;
	uint16_t* layers;
	uint32_t sizes[COMPONENT_LIMIT];
	uint32_t offsets[COMPONENT_LIMIT];
	uint32_t size;
}entity_data;

void ecs_init(entity_data* d, uint32_t n, ...);
void ecs_clear(entity_data* d);
void ecs_deinit(entity_data* d);

void* component_get(entity_data* d, uint32_t eid, uint32_t cid);
void component_add(entity_data* d, uint32_t eid, uint32_t cid, void* data);
void component_remove(entity_data* d, uint32_t eid, uint32_t cid);

void entity_add_flag(entity_data* d, uint32_t eid, uint32_t flag);
uint8_t entity_active(entity_data* d, uint32_t eid);
uint8_t entity_exists_mask(entity_data* d, uint8_t exact, uint64_t mask);
uint8_t entity_exists(entity_data* d, uint8_t exact, uint32_t n, ...);
uint16_t entity_max_layer(entity_data* d);
uint16_t entity_get_layer(entity_data* d, uint32_t eid);
void entity_set_layer(entity_data* d, uint32_t eid, uint16_t layer);
uint32_t entity_nearest_mask(entity_data* d, struct v2 pos, uint8_t exact, uint64_t mask);
uint32_t entity_nearest(entity_data* d, struct v2 pos, uint8_t exact, uint32_t n, ...);
uint32_t entity_nearest_mask_n(entity_data* d, struct v2 pos, uint32_t n, uint8_t exact, uint64_t mask);
uint32_t entity_nearest_n(entity_data* d, struct v2 pos, uint8_t exact, uint32_t n, uint32_t m, ...);
uint32_t entity_furthest_mask(entity_data* d, struct v2 pos, uint8_t exact, uint64_t mask);
uint32_t entity_furthest(entity_data* d, struct v2 pos, uint8_t exact, uint32_t n, ...);
uint32_t entity_furthest_mask_n(entity_data* d, struct v2 pos, uint32_t n, uint8_t exact, uint64_t mask);
uint32_t entity_furthest_n(entity_data* d, struct v2 pos, uint8_t exact, uint32_t n, uint32_t m, ...);
uint32_t entity_create(entity_data* d);
void entity_destroy(entity_data* d, uint32_t eid);

uint8_t entity_exact_mask_logic(entity_data* d, uint32_t eid, uint8_t exact, uint64_t reference, uint64_t candidate);

void entity_create_signal(entity_data* d, uint32_t signal);
void entity_set_signal(entity_data* d, uint32_t signal, float value);
float entity_signal_check(entity_data* d, uint32_t signal);
float entity_receive_signal(entity_data* d, uint32_t signal);

#endif
