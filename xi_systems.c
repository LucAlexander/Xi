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
