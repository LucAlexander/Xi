#include <stdio.h>

#include "project.h"
#include "xi_engine.h"

void project_structs_init(xi_utils* xi){	
	project_structs* project = malloc(sizeof(project_structs));
	xi->project = project;
	// initialize instances of game structures here
}

void xisetup(program_state* state, xi_utils* xi){
	// register your systems here
}

void spawn_player(xi_utils* xi, u32 x, u32 y);
void player_movement(SYSTEM_ARG_REQUIREMENTS);

void xistart(xi_utils* xi){
	// setup initial game state here
	add_animation(xi->graphics, "player_run", 0, 0, 10);
	add_animation(xi->graphics, "player_idle", 0, 2, 12);
	add_animation(xi->graphics, "player_die", 2, 4, 3);

	u32 single_instance_runtime = entity_create(xi->ecs);
	u8 sir = 0;
	component_add(
		xi->ecs,
		single_instance_runtime,
		SINGLE_RUN_CONTROLLER_C,
		&sir
	);

	spawn_player(xi, 128, 128);

}

void spawn_player(xi_utils* xi, u32 x, u32 y){
	u32 entity = entity_create(xi->ecs);
	entity_add_flag(xi->ecs, entity, ENTITY_SOLID);

	Blitable sprite;
	BlitableInitF_arena(xi->graphics, &sprite, XI_SPRITE"catpon.png", 32, 32);

	animator_t animator;
	animator_t_init(&animator);
	animator_set_active(&animator, 1);
	animator_set_loop(&animator, 1);
	animator_set_frame_time(&animator, SECOND/25);
	animator_set_animation(xi->graphics, &animator, "player_idle", &sprite);

	v2 position;
	position.x = x;
	position.y = y;

	v2 forces;
	forces.x = 0;
	forces.y = 0;

	logic_t behavior;
	behavior.f = player_movement;

	v4 collider = {-16, -16, 32, 32};
	
	component_add(xi->ecs, entity, POSITION_C, &position);
	component_add(xi->ecs, entity, FORCES_C, &forces);
	component_add(xi->ecs, entity, BLITABLE_C, &sprite);
	component_add(xi->ecs, entity, BEHAVIOR_C, &behavior);
	component_add(xi->ecs, entity, ANIMATOR_C, &animator);
	component_add(xi->ecs, entity, COLLIDER_C, &collider);
}

void player_movement(SYSTEM_ARG_REQUIREMENTS){
	u8 up, down, left, right;
	up = keyHeld(xi->user_input, "w");
	left = keyHeld(xi->user_input, "a");
	down = keyHeld(xi->user_input, "s");
	right = keyHeld(xi->user_input, "d");

	float hsp, vsp;
	hsp = (right-left)*2;
	vsp = (down-up)*2;

	ARG(v2* forces, FORCES_C);
	forces->x = hsp;
	forces->y = vsp;

	ARG(Blitable* sprite, BLITABLE_C);
	ARG(animator_t* animator, ANIMATOR_C);

	if (hsp != 0 || vsp != 0){
		if ((hsp > 0 && (sprite->flags & BLITABLE_FLIP_H)) || (hsp < 0 && !(sprite->flags & BLITABLE_FLIP_H))){
			sprite->flags ^= BLITABLE_FLIP_H;
		}
		animator_set_animation(xi->graphics, animator, "player_run", sprite);
		return;
	}
	animator_set_animation(xi->graphics, animator, "player_idle", sprite);
}
