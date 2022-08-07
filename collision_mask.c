#include "collision_mask.h"

VECTOR_SOURCE(vline_t, m2)
VECTOR_SOURCE(vpartition_t, vline_t)

void collision_mask_init(collision_mask* m, uint32_t w, uint32_t h, uint32_t depth){
	m->w = w;
	m->h = h;
	m->subdivision_w = w/(1<<depth);
	m->subdivision_h = h/(1<<depth);
	m->subdivision_w_count = w/m->subdivision_w;
	m->subdivision_h_count = h/m->subdivision_h;
	m->partitions = vpartition_tInit();
	uint32_t i, n = m->subdivision_w_count*m->subdivision_h_count;
	vpartition_tReserve(&m->partitions, n);
	for (i = 0;i<n;++i){
		vpartition_tPushBack(&m->partitions, vline_tInit());
	}
}

void collision_mask_add_edge(collision_mask* m, m2 edge){
	uint32_t a = collision_mask_retrieve_index(m, edge.x1, edge.y1);
	uint32_t b = collision_mask_retrieve_index(m, edge.x2, edge.y2);
	if (a==b){
		vline_tPushBack(vpartition_tRef(&m->partitions, b), edge);
	}
	vline_tPushBack(vpartition_tRef(&m->partitions, a), edge);
}

vline_t collision_mask_retrieve_partition(collision_mask* m, v2 point){
	return vpartition_tGet(&m->partitions, collision_mask_retrieve_index(m, point.x, point.y));
}

uint32_t collision_mask_retrieve_index(collision_mask* m, uint32_t x, uint32_t y){
	return (x/m->subdivision_w) + (m->subdivision_w_count*(y/m->subdivision_h));
}

void collision_mask_free(collision_mask* m){
	uint32_t i, n = m->partitions.size;
	for (i = 0;i<n;++i){
		vline_tFree(vpartition_tRef(&m->partitions, i));
	}
	vpartition_tFree(&m->partitions);
}

