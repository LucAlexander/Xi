#include <stdio.h>
#include <time.h>

#include "xi.h"
#include "config/project-config.h"

#include "mathutils.h"
#include "graphicsutils.h"

#include "xi_components.h"
#include "xi_systems.h"

#include "systems.h"

VECTOR_SOURCE(vsys_t, system_t)
HASHMAP_SOURCE(mu32u8_t, uint32_t, uint8_t, hashI)

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
	audio_init(&state->audio);
	inputInit(&state->user_input);
	ecs_init(&state->ecs, COMPONENT_COUNT, COMPONENT_SIZES);
	state->project = NULL;
}

void program_state_deinit(program_state* state){
	uint32_t i;
	for (i = 0;i<XI_SYSTEM_STATE_COUNT;++i){
		vsys_tFree(&state->system_group[i]);
	}
	graphicsClose(&state->graphics);
	audio_close(&state->audio);
	ecs_deinit(&state->ecs);
	free(state->project);
	state->project = NULL;
}

xi_utils construct_xi_utils(program_state* state){
	xi_utils xi = {
		&state->graphics,
		&state->audio,
		&state->user_input,
		&state->ecs,
		state->project,
		state->tick
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

void reset_arena(xi_utils* xi){
	texture_arena_release(xi->graphics);
	xi->graphics->texture_arena = vecT_tInit();
	ecs_clear(xi->ecs);
}

void system_add(program_state* state, system_t system, PROGRAM_STATE software_state){
	if (software_state >= XI_SYSTEM_STATE_COUNT){
		return;
	}
	vsys_tPushBack(&state->system_group[software_state], system);
}

void std_systems(program_state* state){
	system_add(state, system_init(forces_s, 2, POSITION_C, FORCES_C), XI_STATE_UPDATE);
	system_add(state, system_init(behavior_s, 1, BEHAVIOR_C), XI_STATE_UPDATE);
	system_add(state, system_init(repeater_s, 1, REPEATER_C), XI_STATE_UPDATE);
	system_add(state, system_init(animate_s, 2, BLITABLE_C, ANIMATOR_C), XI_STATE_UPDATE);
	system_add(state, system_init(clickable_s, 2, POSITION_C, CLICKABLE_C), XI_STATE_UPDATE);

	system_t blit = system_init(blitable_s, 2, POSITION_C, BLITABLE_C);
	system_add_filter(&blit, 1, ENTITY_GUI);
	system_add(state, blit, XI_STATE_RENDER);

	system_t guiblit = system_init(blitable_s, 2, POSITION_C, BLITABLE_C);
	system_add_requirement(&guiblit, 1, ENTITY_GUI);
	system_add(state, guiblit, XI_STATE_RENDER_GUI);

	system_t text_blit = system_init(text_s, 2, POSITION_C, TEXT_C);
	system_add_filter(&blit, 1, ENTITY_GUI);
	system_add(state, text_blit, XI_STATE_RENDER);

	system_t text_guiblit = system_init(text_s, 2, POSITION_C, TEXT_C);
	system_add_requirement(&text_guiblit, 1, ENTITY_GUI);
	system_add(state, text_guiblit, XI_STATE_RENDER_GUI);
}

void xi_run_system_group(program_state* state, uint32_t group, uint16_t layer){
	state->state = group;
	uint32_t i;
	vsys_t system_list = state->system_group[group];
	xi_utils xi = construct_xi_utils(state);
	for (i = 0;i<system_list.size;++i){
		system_run(vsys_tGet(&system_list, i), &xi, layer);
	}
}

void run_render_systems(program_state* state, uint32_t group){
	uint32_t i, n = state->ecs.entities;
	mu32u8_t map = mu32u8_tInit();
	for (i = 0;i<n;++i){
		mu32u8_tPush(&map, state->ecs.layers[i], 1);
	}
	n = map.size;
	uint32_t* layers = mu32u8_tGetKeySet(&map);
	mu32u8_tFree(&map);
	for (i = 0;i<n;++i){
		xi_run_system_group(state, group, i);
	}
	free(layers);
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
	xi_run_system_group(state, XI_STATE_UPDATE_PRE, 0);
	xi_run_system_group(state, XI_STATE_UPDATE, 0);
	xi_run_system_group(state, XI_STATE_UPDATE_POST, 0);
	newInputFrame(&state->user_input);
	renderClear(&state->graphics);
	renderSetColor(&state->graphics, 0, 0, 0, 255);
	run_render_systems(state, XI_STATE_RENDER);
	view world = renderGetView(&state->graphics);
	renderSetViewAbsolute(&state->graphics);
	run_render_systems(state, XI_STATE_RENDER_GUI);
	renderSetView(&state->graphics, world);
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

