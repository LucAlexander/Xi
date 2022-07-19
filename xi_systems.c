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
