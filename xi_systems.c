#include "xi_systems.h"
#include "xi_components.h"
#include "xi.h"

#include "mathutils.h"
#include "graphicsutils.h"
#include "entities.h"

SYSTEM(forces_s){
	ARG(v2* position, POSITION_C);
	ARG(v2* forces, FORCES_C);
	position->x += forces->x;
	position->y += forces->y;
	forces->x = 0;
	forces->y = 0;
}

SYSTEM(blitable_s){
	ARG(v2* position, POSITION_C);
	ARG(Blitable* sprite, BLITABLE_C);
	renderBlitableV2(xi->graphics, sprite, *position);
}

SYSTEM(behavior_s){
	ARG(logic_t* behavior, BEHAVIOR_C);
	behavior->f(SYSTEM_ARGS);
}

SYSTEM(text_s){
	ARG(v2* position, POSITION_C);
	ARG(text_node_t* node, TEXT_C);
	drawTextC(xi->graphics, position->x, position->y, node->text,
		node->red,
		node->green,
		node->blue,
		node->alpha
	);
}

SYSTEM(repeater_s){
	ARG(repeater_t* wrapper, REPEATER_C);
	wrapper->ticks += xi->ticks;
	if (wrapper->ticks >= wrapper->trigger_time && wrapper->trigger_count > 0){
		wrapper->ticks -= wrapper->trigger_time;
		wrapper->f(SYSTEM_ARGS);
		wrapper->trigger_count -- ;
		if (wrapper->destroy_after && wrapper->trigger_count == 0){
			entity_destroy(xi->ecs, id);
		}
	}
}

SYSTEM(clickable_s){
	ARG(v2* position, POSITION_C);
	ARG(clickable_t* button, CLICKABLE_C);
	if (button->recharge_counter > 0){
		button->recharge_counter -= xi->ticks;
	}
	v2 mouse = mousePos(xi->user_input);
	v4 bounds = {
		position->x,
		position->y,
	       	button->w,
		button->h
	};
	if (
		pointInRectV2(mouse, bounds) &&
		mousePressed(xi->user_input, 1) &&
		button->recharge_counter <= 0
	){
		button->toggle = !button->toggle;
		button->recharge_counter = button->recharge_time;
		button->f(SYSTEM_ARGS);
	}
}

SYSTEM(draw_clickable_s){
	ARG(v2* position, POSITION_C);
	ARG(clickable_t* button, CLICKABLE_C);
	renderSetColor(xi->graphics, 0, 255, 0, 255);
	drawRect(xi->graphics, position->x, position->y, button->w, button->h, OUTLINE);
	renderSetColor(xi->graphics, 0, 0, 0, 255);
}

SYSTEM(animate_s){
	ARG(Blitable* sprite, BLITABLE_C);
	ARG(animator_t* animation, ANIMATOR_C);
	sprite->drawBound.x = animation->pos_x*sprite->drawBound.w;
	sprite->drawBound.y = animation->pos_y*sprite->drawBound.h;
	animation->frame_time_counter += xi->ticks;
	if (animation->frame_time_counter < animation->frame_time){
		return;
	}
	animation->frame_time_counter -= animation->frame_time;
	progress_animation(xi->graphics, animation);
}

SYSTEM(draw_entity_colliders_s){
	ARG(v2* pos, POSITION_C);
	ARG(v4* mask, COLLIDER_C);
	v4 translated = {
		mask->x + pos->x,
	       	mask->y + pos->y,
		mask->w,
		mask->h
	};
	renderSetColor(xi->graphics, 255, 255, 255, 255);
	drawRectV4(xi->graphics, translated, OUTLINE);
	renderSetColor(xi->graphics, 0, 0, 0, 255);
}

SYSTEM(draw_world_colliders_s){
	renderSetColor(xi->graphics, 255, 255, 255, 255);
	spacial_quadtree_node_t* root = xi->colliders;
	vquadnode_t q = vquadnode_tInit();
	vquadnode_tPushBack(&q, root);
	while(q.size != 0){
		root = vquadnode_tPop(&q);
		if (root->state != INTERNAL_NODE){
			if (root->state != 0){
				renderSetColor(xi->graphics, 255, 0, 0, 128);
				drawRectV4(xi->graphics, root->mask, FILL);
				renderSetColor(xi->graphics, 255, 255, 255, 255);
			}
			drawRectV4(xi->graphics, root->mask, OUTLINE);
			continue;
		}
		vquadnode_tInsert(&q, 0, root->a);
		vquadnode_tInsert(&q, 0, root->b);
		vquadnode_tInsert(&q, 0, root->c);
		vquadnode_tInsert(&q, 0, root->d);
	}
	vquadnode_tFree(&q);
	renderSetColor(xi->graphics, 0, 0, 0, 255);
}

SYSTEM(solid_collision_s){
	ARG(v2* pos, POSITION_C);
	ARG(v2* forces, FORCES_C);
	ARG(v4* mask, COLLIDER_C);
	v4 translated = {
		mask->x + pos->x,
		mask->y + pos->y,
		mask->x + pos->x + mask->w,
		mask->y + pos->y + mask->h
	};
	vv4_t colliders = retrieve_quadtree_collisions(xi->colliders, pos->x, pos->y);
	v4 t = translated;
	t.x += forces->x;
	t.w += forces->x;
	if (collides_with_mask(t, colliders)){
		uint32_t increment = sign(forces->x);
		int32_t count = 0;
		t.x -= forces->x-increment;
		t.w -= forces->x-increment;
		while (collides_with_mask(t, colliders)){
			t.x += increment;
			t.w += increment;
			count++;
		}
		forces->x = count*increment;
	}
	pos->x += forces->x;
	forces->x = 0;
	t = translated;
	t.y += forces->y;
	t.h += forces->y;
	if (collides_with_mask(t, colliders)){
		uint32_t increment = sign(forces->y);
		int32_t count = 0;
		t.y -= forces->y-increment;
		t.h -= forces->y-increment;
		while (collides_with_mask(t, colliders)){
			t.y += increment;
			t.h += increment;
			count++;
		}
		forces->y = count*increment;
	}
	pos->y += forces->y;
	forces->y = 0;
	vv4_tFree(&colliders);
}
