#ifndef XI_SYSTEMS_H
#define XI_SYSTEMS_H

#include "systems.h"

struct entity_data;

SYSTEM(forces_s);

SYSTEM(blitable_s);
SYSTEM(text_s);

SYSTEM(behavior_s);
SYSTEM(repeater_s);
SYSTEM(clickable_s);
SYSTEM(draw_clickable_s);

SYSTEM(animate_s);

SYSTEM(draw_entity_colliders_s);
SYSTEM(draw_world_colliders_s);

SYSTEM(solid_collision_s);

#endif
