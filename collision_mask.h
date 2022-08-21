#ifndef XI_COLLISION_MASK_H
#define XI_COLLISION_MASK_H

#include "mathutils.h"
#include "vector.h"

VECTOR(vu32_t, uint32_t)
VECTOR(vv4_t, v4)

#define INTERNAL_NODE 777

typedef enum QUADTREE_ADJACENCY_DIRECTION{
	LEFT=0,
	DOWN=1,
	RIGHT=2,
	UP=3
}QUADTREE_ADJACENCY_DIRECTION;

typedef struct spacial_quadtree_node_t{
	struct spacial_quadtree_node_t* a;
	struct spacial_quadtree_node_t* b;
	struct spacial_quadtree_node_t* c;
	struct spacial_quadtree_node_t* d;
	uint32_t state;
	uint32_t depth;
	v4 mask;
}spacial_quadtree_node_t;

VECTOR(vquadnode_t, spacial_quadtree_node_t*)

vu32_t get_pixel_color_data(int32_t bytes_per_pixel, uint8_t* pixel, int32_t w, int32_t h);
spacial_quadtree_node_t* partition_pixel_color_data_to_quadtree(uint32_t x_start, uint32_t y_start, uint32_t w, uint32_t h, vu32_t pixel_colors, uint32_t depth, uint32_t absw);
spacial_quadtree_node_t* spacial_quadtree_node_init(uint32_t state);
spacial_quadtree_node_t* generate_collision_mask(const char* src);
void spacial_quadtree_node_free(spacial_quadtree_node_t* root);
spacial_quadtree_node_t* retrieve_quadtree_node(spacial_quadtree_node_t* root, uint32_t x, uint32_t y);
void find_colliding_row(spacial_quadtree_node_t* node, uint32_t y, uint32_t x_min, uint32_t x_max, vv4_t* colliders);
void find_colliding_col(spacial_quadtree_node_t* node, uint32_t x, uint32_t y_min, uint32_t y_max, vv4_t* colliders);
void find_colliding_partitions(spacial_quadtree_node_t* root, v4 rect, vv4_t* colliders);
uint8_t collides_with_mask(v4 calling, vv4_t* parts);

#endif