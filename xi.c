#include <stdio.h>
#include <time.h>

#include "xi.h"
#include "xi_config.h"

#include "xi_components.h"
#include "xi_systems.h"

#include "memarena.h"
#include "systems.h"

VECTOR_SOURCE(vsys_t, system_t)

void program_state_init(program_state* state){
	state->running = 1;
	state->base_time = 0;
	state->tick = 0;
	uint32_t i;
	for (i = 0;i<XI_SYSTEM_STATE_COUNT;++i){
		state->system_group[i] = vsys_tInit();
	}
	state->state = XI_SYSTEM_STATE_COUNT; // pre state sentinel
	graphicsInit(&state->graphics, WINDOW_W, WINDOW_H, WINDOW_TITLE);
	inputInit(&state->user_input);
	entity_data_init(&state->ecs);
	state->arena = mem_arena_alloc(MEM_POOL_INITIAL, MEM_POOL_RESIZE, MEM_POOL_RESIZE_COUNT);
}

void program_state_deinit(program_state* state){
	uint32_t i, k;
	for (i = 0;i<XI_SYSTEM_STATE_COUNT;++i){
		vsys_t system_list = state->system_group[i];
		for (k = 0;k<system_list.size;++k){
			system_free(vsys_tRef(&system_list, k));
		}
		vsys_tFree(&system_list);
	}
	graphicsClose(&state->graphics);
	entity_data_free(&state->ecs);
	mem_arena_dealloc(state->arena);
}

xi_utils construct_xi_utils(program_state* state){
	xi_utils xi = {
		&state->graphics,
		&state->user_input,
		&state->ecs,
		state->arena
	};
	return xi;
}

void xi_init(program_state* state){
	state->state = XI_STATE_INIT;
	srand(time(NULL));
	loadFont(&state->graphics, "../fnt/arcade.TTF", "default");
	setFont(&state->graphics, "default");
	view v = {
		0,        0,
		0,        0,
		WINDOW_W, WINDOW_H
	};
	renderSetView(&state->graphics, v);
	std_systems(state);
	xi_utils xi = construct_xi_utils(state);
	xisetup(&xi);
	xistart(&xi);
}

void xi_deinit(program_state* state){
	state->state = XI_STATE_DEINIT;
}

void xi_purge(program_state* state){
	state->state = XI_STATE_PURGE;
	purge(&state->ecs);
}

void system_add(program_state* state, system_t system, PROGRAM_STATE software_state){
	if (software_state >= XI_SYSTEM_STATE_COUNT){
		return;
	}
	vsys_tPushBack(&state->system_group[software_state], system);
}

void std_systems(program_state* state){
	system_add(state, system_init(forces_s, 2, POSITION_C_MOC, FORCES_C_MOC), XI_STATE_UPDATE);
	system_add(state, system_init(blitable_s, 2, POSITION_C_MOC, BLITABLE_C_MOC), XI_STATE_RENDER);
}

void xi_run_system_group(program_state* state, uint32_t group, int32_t layer){
	state->state = group;
	uint32_t i;
	vsys_t system_list = state->system_group[group];
	xi_utils xi = construct_xi_utils(state);
	for (i = 0;i<system_list.size;++i){
		system_run(vsys_tRef(&system_list, i), &xi, layer);
	}
}

void run_render_systems(program_state* state, uint32_t group){
	mu32_u32Iterator it = mu32_u32IteratorInit(&state->ecs.ent2layer);
	int32_t layer = -1;
	while (mu32_u32IteratorHasNext(&it)){
		mu32_u32Result r = mu32_u32IteratorNext(&it);
		if (r.val != layer){
			layer = r.val;
			xi_run_system_group(state, group, layer);
		}
	}
}

void tick_update(program_state* state){
	state->tick = (SDL_GetTicks()-state->base_time);
}

void tick_reset(program_state* state){
	state->base_time = SDL_GetTicks();
	state->tick = 0;
}

uint8_t tick(program_state* state){
	return state->tick >= TICK_TIME;
}

uint32_t frame_time(program_state* state){
	return state->tick;
}

void do_frame_try(program_state* state){
	while (SDL_PollEvent(&state->event)){
		read_user_input(state);
	}
	tick_update(state);
	if (!tick(state)){
		return;
	}
	xi_run_system_group(state, XI_STATE_UPDATE_PRE, LAYER_INDEPENDENT);
	xi_run_system_group(state, XI_STATE_UPDATE, LAYER_INDEPENDENT);
	xi_run_system_group(state, XI_STATE_UPDATE_POST, LAYER_INDEPENDENT);
	xi_purge(state);
	newInputFrame(&state->user_input);
	renderClear(&state->graphics);
	renderSetColor(&state->graphics, 0, 0, 0, 255);
	run_render_systems(state, XI_STATE_RENDER);
	run_render_systems(state, XI_STATE_RENDER_GUI);
	renderFlip(&state->graphics);
	tick_reset(state);
}


void read_user_input(program_state* state){
	switch (state->event.type){
		case SDL_QUIT:
			state->running = 0;
			return;
		case SDL_KEYDOWN:
			if (state->event.key.repeat == 0){
				keyDownEvent(&state->user_input, state->event);
			}
			return;
		case SDL_KEYUP:
			keyUpEvent(&state->user_input, state->event);
			return;
		case SDL_MOUSEBUTTONDOWN:
			if (!mouseHeld(&state->user_input, state->event.button.button)){
				mouseDownEvent(&state->user_input, state->event);
			}
			return;
		case SDL_MOUSEBUTTONUP:
			mouseUpEvent(&state->user_input, state->event);
			return;
		case SDL_MOUSEWHEEL:
			mouseScrollEvent(&state->user_input, state->event.wheel.y);
			return;
		case SDL_MOUSEMOTION:
			mouseMoveEvent(&state->user_input, state->event.motion.x, state->event.motion.y);
			return;
	}
}

int main(int argc, char** argv){
	program_state state;
	program_state_init(&state);
	xi_init(&state);
	tick_reset(&state);
	while (state.running){
		do_frame_try(&state);
	}
	xi_deinit(&state);
	program_state_deinit(&state);
	return 0;
}

