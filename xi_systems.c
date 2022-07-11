#include "xi_systems.h"
#include "xi_components.h"
#include "xi.h"

#include "mathutils.h"
#include "graphicsutils.h"
#include "entities.h"

SYSTEM(forces_s){
	ARG(v2* position, POSITION_C_MOC);
	ARG(v2* forces, FORCES_C_MOC);
	position->x += forces->x;
	position->y += forces->y;
}

SYSTEM(blitable_s){
	ARG(v2* position, POSITION_C_MOC);
	ARG(Blitable* sprite, BLITABLE_C_MOC);
	renderBlitableV2(xi->graphics, sprite, *position);
}

SYSTEM(behavior_s){
	ARG(void(*f)(SYSTEM_ARG_REQUIREMENTS), BEHAVIOR_C_MOC);
	f(SYSTEM_ARGS);
}

SYSTEM(repeater_s){
	ARG(repeater_t* wrapper, REPEATER_C_MOC);
	wrapper->ticks += xi->ticks;
	if (wrapper->ticks >= wrapper->trigger_time && wrapper->trigger_count > 0){
		wrapper->ticks -= wrapper->trigger_time;
		wrapper->f(SYSTEM_ARGS);
		wrapper->trigger_count -- ;
	}
}
