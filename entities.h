#ifndef ENTITIES_H
#define ENTITIES_H

#include "vector.h"
#include "hashMap.h"
#include "bitmasks.h"

VECTOR(vec_t, void*)
VECTOR(mat_t, vec_t)

typedef struct archetype_v2{
	vu64_t mask;
	mat_t data;
	vu32_t ids;
}archetype_v2;

void freeArchetype(archetype_v2* e);

VECTOR(varch_t, archetype_v2)
VECTOR(qu32_t, uint32_t)
HASHMAP(mu32_mask, uint32_t, uint64_t)
HASHMAP(mu32_u32, uint32_t, uint32_t)

typedef enum ENTITY_FLAGS{
	ENTITY_DEACTIVATED=1
}ENTITY_FLAGS;

typedef struct entity_data{
	varch_t archetypes;
	uint32_t eid_max;
	uint32_t entity_count;
	qu32_t eid_backlog;
	mu32_mask masks; // entity flags
	mu32_u32 ent2arch;
	mu32_u32 ent2layer;
}entity_data;

void entity_data_display(entity_data* data);
void displayArchetype(entity_data* data, archetype_v2* arch, uint32_t index);

void entity_data_init(entity_data*);
void addDefaultArchetype(entity_data* data);
void entity_data_free(entity_data*);

uint32_t form_entity(entity_data*);
void destroy_entity(entity_data*, uint32_t eid);
void purge(entity_data* data);
uint8_t purgeEntity(entity_data* data, uint32_t eid); 

uint8_t entity_exists(entity_data*, uint32_t eid);

vec_t* entity_reference(entity_data* data, uint32_t eid);

void tether_component(entity_data*, uint32_t eid, uint32_t cid, void* data);
void addComponentToExistingEntity(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, uint32_t cid, uint32_t index, void* component_data);
uint8_t swapToExistingArchetype(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* oldArch, vu64_t* newmask, uint32_t index);
void swapToNewArchetype(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, vu64_t newmask, uint32_t index);

void untether_component(entity_data*, uint32_t eid, uint32_t cid);
uint8_t removeComponentFromEntity(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, uint32_t cid, uint32_t index);

int32_t get_entity_layer(entity_data*, uint32_t eid);
void set_entity_layer(entity_data*, uint32_t eid, uint32_t layer);

/* data[
 *	{archetype mask : [component list], [component list]}
 *]
*/


#endif
