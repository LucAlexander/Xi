#include "collision_mask.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

VECTOR_SOURCE(vu32_t, uint32_t)
VECTOR_SOURCE(vv4_t, v4)
VECTOR_SOURCE(vquadnode_t, spacial_quadtree_node_t*)

spacial_quadtree_node_t* spacial_quadtree_node_init(uint32_t state){
	spacial_quadtree_node_t* node = malloc(sizeof(spacial_quadtree_node_t));
	node->a= NULL;
	node->b= NULL;
	node->c= NULL;
	node->d= NULL;
	node->state = state;
	node->depth = 0;
	node->mask.x = 0;
	node->mask.y = 0;
	node->mask.w = 0;
	node->mask.h = 0;
	return node;
}

vu32_t get_pixel_color_data(int32_t bpp, uint8_t* pixels, int32_t w){
	uint32_t b, x, y;
	uint8_t* pixel;
	vu32_t pixel_colors = vu32_tInit();
	for (pixel = pixels,x=0,y=0;pixel != NULL;pixel += bpp){
		uint32_t color = 0;
		for (b = 0;b<bpp;++b){
			color += pixel[b] << (bpp-b);
		}
		vu32_tPushBack(&pixel_colors, color);
		if (++x % w == 0){
			x = 0;
			y++;
		}
	}
	return pixel_colors;
}

void partition_pixel_color_data_to_quadtree(uint32_t x_start, uint32_t y_start, uint32_t w, uint32_t h, vu32_t pixel_colors, spacial_quadtree_node_t* node, uint32_t depth){
	uint32_t color = vu32_tGet(&pixel_colors, 0);
	node = spacial_quadtree_node_init(color);
	node->depth = depth;
	node->mask.x = x_start;
	node->mask.y = y_start;
	node->mask.w = w;
	node->mask.h = h;
	if (w < 1 || h < 1){
		return;
	}
	uint32_t x, y, offset;
	for (y=y_start;y<h;++y){
		for (offset=w*y,x=x_start;x<w;++x){
			if (vu32_tGet(&pixel_colors, x+offset) != color){
				node->state = INTERNAL_NODE;
				uint32_t w2 = w/2;
				uint32_t h2 = h/2;
				depth++;
				partition_pixel_color_data_to_quadtree(x, y, w2, h2, pixel_colors, node->a, depth);
				partition_pixel_color_data_to_quadtree(x+w2, y, w2, h2, pixel_colors, node->b, depth);
				partition_pixel_color_data_to_quadtree(x, y+h2, w2, h2, pixel_colors, node->c, depth);
				partition_pixel_color_data_to_quadtree(x+w2, y+h2, w2, h2, pixel_colors, node->d, depth);
				return;
			}
		}
	}
}

void spacial_quadtree_node_free(spacial_quadtree_node_t* root){
	if (root->state != INTERNAL_NODE){
		free(root);
		root = NULL;
		return;
	}
	spacial_quadtree_node_free(root->a);
	spacial_quadtree_node_free(root->b);
	spacial_quadtree_node_free(root->c);
	spacial_quadtree_node_free(root->d);
	root->a = NULL;
	root->b = NULL;
	root->c = NULL;
	root->d = NULL;
	free(root);
	root = NULL;
}

spacial_quadtree_node_t* generate_collision_mask(const char* src){
	SDL_Surface* item = IMG_Load(src);
	int32_t w = item->w;
	int32_t h = item->h;
	if (item == NULL){
		printf("[!] Unable to load image from file \'%s\'\n%s\n", src, SDL_GetError());
	}
	SDL_LockSurface(item);
	vu32_t pixel_colors = get_pixel_color_data(item->format->BytesPerPixel, item->pixels, w);
	SDL_UnlockSurface(item);
	SDL_FreeSurface(item);
	spacial_quadtree_node_t* root = NULL;
	partition_pixel_color_data_to_quadtree(0, 0, w, h, pixel_colors, root,0);
	vu32_tFree(&pixel_colors);
	return root;
}

spacial_quadtree_node_t* retrieve_quadtree_node(spacial_quadtree_node_t* root, uint32_t x, uint32_t y){
	if (root == NULL || root->state != INTERNAL_NODE){
		return root;
	}
	if (x < root->mask.w/2){
		if (y < root->mask.h/2){
			return retrieve_quadtree_node(root->a, x, y);
		}
		return retrieve_quadtree_node(root->c, x, y);
	}
	if (y < root->mask.h/2){
		return retrieve_quadtree_node(root->b, x, y);
	}
	return retrieve_quadtree_node(root->d, x, y);
}

spacial_quadtree_node_t* retrieve_quadtree_node_at_depth(spacial_quadtree_node_t* root, uint32_t x, uint32_t y, uint32_t depth){
	if (root == NULL || root->state != INTERNAL_NODE || root->depth == depth){
		return root;
	}
	if (x < root->mask.w/2){
		if (y < root->mask.h/2){
			return retrieve_quadtree_node_at_depth(root->a, x, y, depth);
		}
		return retrieve_quadtree_node_at_depth(root->c, x, y, depth);
	}
	if (y < root->mask.h/2){
		return retrieve_quadtree_node_at_depth(root->b, x, y, depth);
	}
	return retrieve_quadtree_node_at_depth(root->d, x, y, depth);
}

void populate_collider_vector(vv4_t* masks, spacial_quadtree_node_t* node, uint32_t direction){
	if (node == NULL){
		return;
	}
	if (node->state != INTERNAL_NODE && node->state != 0){
		vv4_tPushBack(masks, node->mask);
	}
	switch (direction){
		case LEFT:
			populate_collider_vector(masks, node->a, direction);
			populate_collider_vector(masks, node->d, direction);
		break;
		case DOWN:
			populate_collider_vector(masks, node->c, direction);
			populate_collider_vector(masks, node->d, direction);
		break;
		case RIGHT:
			populate_collider_vector(masks, node->b, direction);
			populate_collider_vector(masks, node->d, direction);
		break;
		case UP:
			populate_collider_vector(masks, node->a, direction);
			populate_collider_vector(masks, node->b, direction);
		break;
	}
}

vv4_t retrieve_quadtree_collisions(spacial_quadtree_node_t* root, uint32_t x, uint32_t y){
	vv4_t masks = vv4_tInit();
	spacial_quadtree_node_t* node = retrieve_quadtree_node(root, x, y);
	populate_collider_vector(&masks, retrieve_quadtree_node_at_depth(root, node->mask.x-1, node->mask.y, node->depth), RIGHT);
	populate_collider_vector(&masks, retrieve_quadtree_node_at_depth(root, node->mask.x+node->mask.w+1, node->mask.y, node->depth), LEFT);
	populate_collider_vector(&masks, retrieve_quadtree_node_at_depth(root, node->mask.x, node->mask.y-1, node->depth), DOWN);
	populate_collider_vector(&masks, retrieve_quadtree_node_at_depth(root, node->mask.x, node->mask.y+node->mask.h+1, node->depth), UP);
	return masks;
}

uint8_t collides_with_mask(v4 calling, vv4_t masks){
	uint32_t i;
	for (i = 0;i<masks.size;++i){
		v4 mask = vv4_tGet(&masks, i);
		mask.w+=mask.x;
		mask.h+=mask.y;
		if (rectCollidesB(calling, mask)){
			return 1;
		}
	}
	return 0;
}
