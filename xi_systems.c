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
