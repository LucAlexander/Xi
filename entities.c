#include "entities.h"
#include "asciiEsc.h"
#include "cflags.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

VECTOR_SOURCE(varch_t, archetype_v2)
VECTOR_SOURCE(vec_t, void*)
VECTOR_SOURCE(mat_t, vec_t)
VECTOR_SOURCE(qu32_t, uint32_t)
HASHMAP_SOURCE(mu32_mask, uint32_t, uint64_t, hashI)
HASHMAP_SOURCE(mu32_u32, uint32_t, uint32_t, hashI)

void entity_data_init(entity_data* data){
	data->archetypes = varch_tInit();
	data->eid_backlog = qu32_tInit();
	data->eid_max = 0;
	data->entity_count = 0;
	data->masks = mu32_maskInit();
	data->ent2arch = mu32_u32Init();
	data->ent2layer = mu32_u32Init();
	addDefaultArchetype(data);
}

void addDefaultArchetype(entity_data* data){
	archetype_v2 def;
	def.mask = createMask(0);
	def.data = mat_tInit();
	def.ids = vu32_tInit();
	varch_tPushBack(&data->archetypes, def);
}

void entity_data_free(entity_data* data){
	uint32_t i;
	for (i = 0;i<data->archetypes.size;++i){
		archetype_v2* e = varch_tRefTrusted(&data->archetypes, i);
		freeArchetype(e);
	}
	varch_tFree(&data->archetypes);
	qu32_tFree(&data->eid_backlog);
	mu32_maskFree(&data->masks);
	mu32_u32Free(&data->ent2arch);
	mu32_u32Free(&data->ent2layer);
}

void freeArchetype(archetype_v2* e){
	uint32_t k;
	for (k = 0;k<e->data.size;++k){
		vec_t* v = mat_tRefTrusted(&e->data, k);
		vec_tFree(v);
	}
	mat_tFree(&e->data);
	vu64_tFree(&e->mask);
	vu32_tFree(&e->ids);
}

uint32_t form_entity(entity_data* data){
	archetype_v2* def = varch_tRefTrusted(&data->archetypes, 0);
	vec_t block = vec_tInit();
	mat_tPushBack(&def->data, block);
	uint32_t id = data->entity_count++;
	if (data->eid_backlog.size > 0){
		id = qu32_tPop(&data->eid_backlog);
	}
	vu32_tPushBack(&def->ids, id);
	mu32_u32Push(&data->ent2arch, id, 0);
	mu32_u32Push(&data->ent2layer, id, -1);
	if (!mu32_maskContains(&data->masks, id)){
		mu32_maskPush(&data->masks, id, 0);
		return id;
	}
	*mu32_maskRef(&data->masks, id) = 0;
	return id; 
}

void destroy_entity(entity_data* data, uint32_t eid){
	uint64_t* mask = mu32_maskRef(&data->masks, eid);
	*mask = bit_on(*mask, ENTITY_DEACTIVATED);
}

void purge(entity_data* data){
	uint32_t i;
	uint32_t* keys = mu32_maskGetKeySet(&data->masks);
	for (i = 0;i<data->masks.size;++i){
		uint32_t eid = keys[i];
		uint64_t* mask = mu32_maskRef(&data->masks, i);
		if (bit_check(*mask, ENTITY_DEACTIVATED)){
			purgeEntity(data, eid);
		}
	}
	free(keys);
}

uint8_t purgeEntity(entity_data* data, uint32_t eid){
	archetype_v2* arch = varch_tRef(&data->archetypes, mu32_u32Get(&data->ent2arch, eid).val);
	uint32_t i;
	for (i = 0;i<arch->ids.size;++i){
		if (vu32_tGet(&arch->ids, i)==eid){
			data->entity_count--;
			qu32_tPushBack(&data->eid_backlog, eid);
			vu32_tRemove(&arch->ids, i);
			vec_tFree(mat_tRefTrusted(&arch->data, i));
			mat_tRemove(&arch->data, i);
			mu32_u32Pop(&data->ent2arch, eid);
			mu32_u32Pop(&data->ent2layer, eid);
			return 1;
		}
	}
	return 0;
}

uint8_t entity_exists(entity_data* data, uint32_t eid){
	return mu32_u32Contains(&data->ent2arch, eid);
}

vec_t* entity_reference(entity_data* data, uint32_t eid){
	archetype_v2* arch = varch_tRef(&data->archetypes, mu32_u32Get(&data->ent2arch, eid).val);
	uint32_t i;
	for (i = 0;i<arch->ids.size;++i){
		if (vu32_tGet(&arch->ids, i) == eid){
			return mat_tRefTrusted(&arch->data, i);
		}
	}
	return NULL;
}

void tether_component(entity_data* data, uint32_t eid, uint32_t cid, void* component_data){
	archetype_v2* arch = varch_tRef(&data->archetypes, mu32_u32Get(&data->ent2arch, eid).val);
	uint32_t k;
	for (k = 0;k<arch->ids.size;++k){
		if (vu32_tGet(&arch->ids, k)==eid){
			addComponentToExistingEntity(&data->ent2arch, &data->archetypes, arch, cid, k, component_data);
			return;
		}
	}
}

void addComponentToExistingEntity(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, uint32_t cid, uint32_t index, void* component_data){
	vu64_t newmask = maskCopy(&arch->mask);
	uint32_t popcount = 0;
	int32_t t;
	for (t = cid-1;t>=0;--t){
		popcount += maskContainsBit(&newmask, t);
	}
	if (maskContainsBit(&newmask, cid)){
		vu64_tFree(&newmask);
		vec_tSet(mat_tRef(&arch->data, index), popcount, component_data);
		return;
	}
	maskAddBit(&newmask, cid);
	vec_tInsert(mat_tRef(&arch->data, index), popcount, component_data);
	if (swapToExistingArchetype(ent2arch, archetypes, arch, &newmask, index)){
		return;
	}
	swapToNewArchetype(ent2arch, archetypes, arch, newmask, index);
}

uint8_t swapToExistingArchetype(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* oldArch, vu64_t* newmask, uint32_t index){
	int32_t t;
	for (t = 0;t<archetypes->size;++t){
		archetype_v2* existingarc = varch_tRef(archetypes, t);
		if (maskEquals(newmask, &existingarc->mask)){
			mat_tPushBack(&existingarc->data, mat_tRemove(&oldArch->data, index));
			uint32_t eid = vu32_tRemove(&oldArch->ids, index);
			vu32_tPushBack(&existingarc->ids, eid);
			mu32_u32Push(ent2arch, eid, t);
			vu64_tFree(newmask);
			return 1;
		}
	}
	return 0;
}

void swapToNewArchetype(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, vu64_t newmask, uint32_t index){
	archetype_v2 newarc;
	newarc.mask = newmask;
	newarc.data = mat_tInit();
	newarc.ids = vu32_tInit();
	mat_tPushBack(&newarc.data, mat_tRemove(&arch->data, index));
	uint32_t eid = vu32_tRemove(&arch->ids, index);
	vu32_tPushBack(&newarc.ids, eid);
	varch_tPushBack(archetypes, newarc);
	mu32_u32Push(ent2arch, eid, archetypes->size-1);
}

void untether_component(entity_data* data, uint32_t eid, uint32_t cid){
	uint32_t k;
	archetype_v2* arch = varch_tRef(&data->archetypes, mu32_u32Get(&data->ent2arch, eid).val);
	for (k = 0;k<arch->ids.size;++k){
		if ((vu32_tGet(&arch->ids, k)==eid) &&
		(removeComponentFromEntity(&data->ent2arch, &data->archetypes, arch, cid, k))){
			return;
		}
	}
}

uint8_t removeComponentFromEntity(mu32_u32* ent2arch, varch_t* archetypes, archetype_v2* arch, uint32_t cid, uint32_t index){
	vu64_t newmask = maskCopy(&arch->mask);
	if (maskContainsBit(&newmask, cid)){
		maskRemoveBit(&newmask, cid);
		uint32_t popcount = 0;
		int32_t t;
		for (t = cid-1;t>=0;--t){
			popcount += maskContainsBit(&newmask, t);
		}
		vec_t* entity = mat_tRef(&arch->data, index);
		vec_tRemoveInOrder(entity, popcount);
		swapToExistingArchetype(ent2arch, archetypes,arch, &newmask, index);
		vu64_tFree(&newmask);
		return 1;
	}
	vu64_tFree(&newmask);
	return 0;
}

int32_t get_entity_layer(entity_data* data, uint32_t eid){
	return mu32_u32Get(&data->ent2layer, eid).val;
}

void set_entity_layer(entity_data* data, uint32_t eid, uint32_t layer){
	mu32_u32Push(&data->ent2layer, eid, layer);
}

void entity_data_display(entity_data* data){
	printf(A_YELLOW_F" ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n"A_RESET);
	uint32_t i;
	printf(" "A_YELLOW_F "█\n"A_RESET);
	printf(" "A_YELLOW_F"█"A_RESET" "A_YELLOW_F"ARCHETYPE DATA"A_RESET"\n");
	printf(" "A_YELLOW_F "█\n"A_RESET);
	for (i = 0;i<data->archetypes.size;++i){
		archetype_v2* arch = varch_tRef(&data->archetypes, i);
		displayArchetype(data, arch, i);
		printf(" "A_YELLOW_F"█\n"A_RESET);
	}
	printf(" "A_YELLOW_F"█\n"A_RESET" "A_YELLOW_F"█"A_RESET" "A_YELLOW_F"ENTITY ID TO ARCHETYPE TRACKER"A_RESET"\n");
	mu32_u32Iterator it = mu32_u32IteratorInit(&data->ent2arch);
	printf(" "A_YELLOW_F "█\n"A_RESET);
	printf(" "A_YELLOW_F"█ "A_GREEN_F"█ entity id, archetype id, mask\n"A_RESET);
	while (mu32_u32IteratorHasNext(&it)){
		mu32_u32Result r = mu32_u32IteratorNext(&it);
		if (entity_exists(data, r.key)){
			vu64_t mask = varch_tRef(&data->archetypes, r.val)->mask;
			printf(" "A_YELLOW_F"█ "A_GREEN_F"█ "A_RESET"%d"A_GREEN_F", "A_RESET"%d"A_GREEN_F", "A_RESET, r.key, r.val);
			maskDisplay(&mask);
			printf(""A_RESET"");
		}
	}
	printf(A_YELLOW_F" █▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n"A_RESET);
}

void displayArchetype(entity_data* data, archetype_v2* arch, uint32_t index){
	printf(" "A_YELLOW_F"█ "A_CYAN_F"█ Archetype "A_RESET"%d\n", index);
	printf(" "A_YELLOW_F"█ "A_CYAN_F"█ Mask "A_RESET);
	maskDisplay(&arch->mask);
	printf(""A_RESET"");
	uint32_t i, k, n;
	for (i = 0;i<arch->ids.size;++i){
		printf(" "A_YELLOW_F"█ "A_CYAN_F"█\n");
		uint32_t id = vu32_tGet(&arch->ids, i);
		printf(" "A_YELLOW_F"█ "A_CYAN_F"█ "A_MAGENTA_F"█ eid "A_RESET"%u\n", id);
		vec_t* components = mat_tRef(&arch->data, i);
		n = components->size;
		for (k = 0;k<n;++k){
			void* ptr = vec_tGetTrusted(components, k);
			printf(" "A_YELLOW_F"█ "A_CYAN_F"█ "A_MAGENTA_F"█ %p"A_RESET"\n", ptr);
		}
	}
}
