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

vu32_t get_pixel_color_data(int32_t bpp, uint8_t* pixels, int32_t w, int32_t h){
	uint32_t i, b, n = w*h*bpp;
	vu32_t pixel_colors = vu32_tInit();
	for (i = 0;i<n;i+=bpp){
		uint32_t color = 0;
		for (b = 1;b<=bpp;++b){
			color += pixels[i+b-1] << (8*(bpp-b));
		}
		// COMPILED FOR BIG ENDIAN BYTE ORDER
		color *= ((color & 0xFF) != 0);
		vu32_tPushBack(&pixel_colors, color);
	}
	return pixel_colors;
}

spacial_quadtree_node_t* partition_pixel_color_data_to_quadtree(uint32_t x_start, uint32_t y_start, uint32_t w, uint32_t h, vu32_t pixel_colors, uint32_t depth, uint32_t absw){
	uint32_t color = pixel_colors.data[x_start + (y_start*absw)];
	spacial_quadtree_node_t* node = spacial_quadtree_node_init(color);
	node->depth = depth;
	node->mask.x = x_start;
	node->mask.y = y_start;
	node->mask.w = w;
	node->mask.h = h;
	if (w < 1 || h < 1){
		return node;
	}
	uint32_t x, y, offset;
	for (y=y_start;y<y_start+h;++y){
		offset = y*absw;
		for (x=x_start;x<x_start+w;++x){
			if (pixel_colors.data[x+offset] != color){
				node->state = INTERNAL_NODE;
				uint32_t w2 = w/2;
				uint32_t h2 = h/2;
				depth++;
				node->a = partition_pixel_color_data_to_quadtree(x_start, y_start, w2, h2, pixel_colors, depth, absw);
				node->b = partition_pixel_color_data_to_quadtree(x_start+w2, y_start, w2, h2, pixel_colors, depth, absw);
				node->c = partition_pixel_color_data_to_quadtree(x_start, y_start+h2, w2, h2, pixel_colors, depth, absw);
				node->d = partition_pixel_color_data_to_quadtree(x_start+w2, y_start+h2, w2, h2, pixel_colors, depth, absw);
				return node;
			}
		}
	}
	return node;
}

void spacial_quadtree_node_free(spacial_quadtree_node_t* root){
	if (root == NULL) return;
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
	vu32_t pixel_colors = get_pixel_color_data(item->format->BytesPerPixel, item->pixels, w, h);
	SDL_UnlockSurface(item);
	SDL_FreeSurface(item);
	spacial_quadtree_node_t* root = partition_pixel_color_data_to_quadtree(0, 0, w, h, pixel_colors,0, w);
	vu32_tFree(&pixel_colors);
	return root;
}

void find_colliding_row(spacial_quadtree_node_t* node, uint32_t y, uint32_t x_min, uint32_t x_max, vv4_t* colliders){
	if (node == NULL){
		return;
	}
	if (
		(y >= node->mask.y && y <= node->mask.y+node->mask.h) &&(
			(x_min <= node->mask.x && x_max >= node->mask.x) ||
			(x_min >= node->mask.x && x_max <= node->mask.x+node->mask.w) ||
			(x_min <= node->mask.x+node->mask.w && x_max >= node->mask.x+node->mask.w)
		)
	){
		if (node->state != INTERNAL_NODE && node->state != 0){
			vv4_tPushBack(colliders, node->mask);
			return;
		}
		find_colliding_row(node->a, y, x_min, x_max, colliders);
		find_colliding_row(node->b, y, x_min, x_max, colliders);
		find_colliding_row(node->c, y, x_min, x_max, colliders);
		find_colliding_row(node->d, y, x_min, x_max, colliders);
	}
}

void find_colliding_col(spacial_quadtree_node_t* node, uint32_t x, uint32_t y_min, uint32_t y_max, vv4_t* colliders){
	if (node == NULL){
		return;
	}
	if (
		(x >= node->mask.x && x <= node->mask.x+node->mask.w) &&(
			(y_min <= node->mask.y && y_max >= node->mask.y) ||
			(y_min >= node->mask.y && y_max <= node->mask.y+node->mask.h) ||
			(y_min <= node->mask.y+node->mask.h && y_max >= node->mask.y+node->mask.h)
		)
	){
		if (node->state != INTERNAL_NODE && node->state != 0){
			vv4_tPushBack(colliders, node->mask);
			return;
		}
		find_colliding_col(node->a, x, y_min, y_max, colliders);
		find_colliding_col(node->b, x, y_min, y_max, colliders);
		find_colliding_col(node->c, x, y_min, y_max, colliders);
		find_colliding_col(node->d, x, y_min, y_max, colliders);
	}
}

void find_colliding_partitions(spacial_quadtree_node_t* root, v4 rect, vv4_t* colliders){
	find_colliding_row(root, rect.y, rect.x, rect.w, colliders);
	find_colliding_row(root, rect.h, rect.x, rect.w, colliders);
	find_colliding_col(root, rect.x, rect.y, rect.h, colliders);
	find_colliding_col(root, rect.w, rect.y, rect.h, colliders);
}

spacial_quadtree_node_t* retrieve_quadtree_node(spacial_quadtree_node_t* root, uint32_t x, uint32_t y){
	if (root == NULL || root->state != INTERNAL_NODE){
		return root;
	}
	if (x < root->mask.x + root->mask.w/2){
		if (y < root->mask.y + root->mask.h/2){
			return retrieve_quadtree_node(root->a, x, y);
		}
		return retrieve_quadtree_node(root->c, x, y);
	}
	if (y < root->mask.y + root->mask.h/2){
		return retrieve_quadtree_node(root->b, x, y);
	}
	return retrieve_quadtree_node(root->d, x, y);
}

uint8_t collides_with_mask(v4 calling, vv4_t* parts){
	for (uint32_t i = 0;i<parts->size;++i){
		v4 mask = parts->data[i];
		mask.w += mask.x;
		mask.h += mask.y;
		if (rectCollidesB(calling, mask)){
			return 1;
		}
	}
	return 0;
}
