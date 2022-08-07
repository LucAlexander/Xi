#ifndef XI_COLLISION_MASK_H
#define XI_COLLISION_MASK_H

#include "mathutils.h"
#include "vector.h"

#define COLLISION_MASK_DEFAULT_DEPTH 3

VECTOR(vline_t, m2)
VECTOR(vpartition_t, vline_t)

typedef struct collision_mask{
	uint32_t w;
	uint32_t h;
	uint32_t subdivision_w;
	uint32_t subdivision_h;
	uint32_t subdivision_w_count;
	uint32_t subdivision_h_count;
	vpartition_t partitions;
}collision_mask;

void collision_mask_init(collision_mask* m, uint32_t w, uint32_t h, uint32_t depth);
void collision_mask_add_edge(collision_mask* m, m2 edge);
void collision_mask_free(collision_mask* m);

vline_t collision_mask_retrieve_partition(collision_mask* m, v2 point);
uint32_t collision_mask_retrieve_index(collision_mask* m, uint32_t x, uint32_t y);

#endif
