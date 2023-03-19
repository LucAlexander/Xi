#include <stdio.h>
#include <time.h>

#include "xi.h"
#include "config/project-config.h"

#include "mathutils.h"
#include "graphicsutils.h"

#include "xi_components.h"
#include "xi_systems.h"

#include "systems.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

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
	//state->colliders = generate_collision_mask(XI_SPRITE"collision_mask_advanced.png"); ssh this is WIP
	state->colliders = NULL;
	state->project = NULL;
	state->debug = 0;
}

void program_state_deinit(program_state* state){
	uint32_t i;
	for (i = 0;i<XI_SYSTEM_STATE_COUNT;++i){
		vsys_tFree(&state->system_group[i]);
	}
	graphicsClose(&state->graphics);
	audio_close(&state->audio);
	ecs_deinit(&state->ecs);
	spacial_quadtree_node_free(state->colliders);
	free(state->project);
	state->project = NULL;
}

xi_utils construct_xi_utils(program_state* state){
	xi_utils xi = {
		&state->graphics,
		&state->audio,
		&state->user_input,
		&state->ecs,
		state->colliders,
		state->project,
		state->tick,
		&state->debug,
		&state->running
	};
	return xi;
}

void xi_init(program_state* state){
	state->state = XI_STATE_INIT;
	srand(time(NULL));
#ifdef __EMSCRIPTEN__
	loadFont(&state->graphics, "./fnt/arcade.TTF", "default");
#else
	loadFont(&state->graphics, "../fnt/arcade.TTF", "default");
#endif
	setFont(&state->graphics, "default");
	std_systems(state);
	xi_utils xi = construct_xi_utils(state);
	xi_entities_spawn(&xi);
	xisetup(state, &xi);
	xistart(&xi);
	state->project = xi.project;
}

void xi_persistent(SYSTEM_ARG_REQUIREMENTS){
	if (keyPressed(xi->user_input, "`") ){
		*xi->debug = !*xi->debug;
	}
}

void xi_entities_spawn(xi_utils* xi){
	uint32_t single_runner = entity_create(xi->ecs);
	uint8_t rc = 0;
	logic_t logic;
	logic.f = xi_persistent;
	component_add(xi->ecs, single_runner, SINGLE_RUN_CONTROLLER_C, &rc);
	component_add(xi->ecs, single_runner, BEHAVIOR_C, &logic);
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
	system_add(state, system_init(behavior_s, 1, BEHAVIOR_C), XI_STATE_UPDATE);
	system_add(state, system_init(repeater_s, 1, REPEATER_C), XI_STATE_UPDATE);
	system_add(state, system_init(animate_s, 2, BLITABLE_C, ANIMATOR_C), XI_STATE_UPDATE);
	system_add(state, system_init(clickable_s, 2, POSITION_C, CLICKABLE_C), XI_STATE_UPDATE);

	system_t forces_nonsolid = system_init(forces_s, 2, POSITION_C, FORCES_C);
	system_add_filter(&forces_nonsolid, 1, ENTITY_SOLID);
	system_add(state, forces_nonsolid, XI_STATE_UPDATE);

	system_t forces_solid = system_init(solid_collision_s, 3, POSITION_C, FORCES_C, COLLIDER_C);
	system_add_requirement(&forces_solid, 1, ENTITY_SOLID);
	system_add(state, forces_solid, XI_STATE_UPDATE);

	system_add(state, system_init(blitable_s, 2, POSITION_C, BLITABLE_C), XI_STATE_RENDER);
	system_add(state, system_init(draw_world_colliders_s, 1, SINGLE_RUN_CONTROLLER_C), XI_STATE_RENDER);
	system_add(state, system_init(draw_entity_colliders_s, 2, POSITION_C, COLLIDER_C), XI_STATE_RENDER);
	system_add(state, system_init(draw_clickable_s, 2, POSITION_C, CLICKABLE_C), XI_STATE_RENDER);

	system_add(state, system_init(text_s, 2, POSITION_C, TEXT_C), XI_STATE_RENDER);
}

void xi_run_system_group(program_state* state, uint32_t group){
	state->state = group;
	uint32_t i;
	vsys_t system_list = state->system_group[group];
	xi_utils xi = construct_xi_utils(state);
	for (i = 0;i<system_list.size;++i){
		system_run(system_list.data[i], &xi);
	}
}

void xi_run_system_group_queued(program_state* state, uint32_t group){
	state->state = group;
	uint32_t i;
	vsys_t system_list = state->system_group[group];
	xi_utils xi = construct_xi_utils(state);
	for (i = 0;i<system_list.size;++i){
		system_run_queued(system_list.data[i], &xi, &xi.graphics->render_order);
	}
	uint8_t gui = 0;
	view original = renderGetView(xi.graphics);
	while (xi.graphics->render_order.size != 0){
		if ((renderq_min(&xi.graphics->render_order) >= RENDER_GUI_DEPTH) &&( !gui)){
			renderSetViewAbsolute(xi.graphics);
			gui = 1;
		}
		renderq_entry_t data = renderq_pop(&xi.graphics->render_order);
		data.f(data.xi, data.id);
	}
	if (gui){
		renderSetView(xi.graphics, original);
	}
}

void tick_reset(program_state* state){
	state->tick = SDL_GetTicks()-state->base_time;
	state->base_time = SDL_GetTicks();
}

#ifdef __EMSCRIPTEN__
void do_frame_try(void* state_arg){
	program_state* state = state_arg;
#else
void do_frame_try(program_state* state){
#endif
	while (SDL_PollEvent(&state->event)){
		read_user_input(state);
	}
	xi_run_system_group(state, XI_STATE_UPDATE_PRE);
	xi_run_system_group(state, XI_STATE_UPDATE);
	xi_run_system_group(state, XI_STATE_UPDATE_POST);
	renderClear(&state->graphics);
	renderSetColor(&state->graphics, 0, 0, 0, 255);
	xi_run_system_group_queued(state, XI_STATE_RENDER);
	renderFlip(&state->graphics);
	newInputFrame(&state->user_input);
	tick_reset(state);
#ifdef __EMSCRIPTEN__
	if (state->running) return;
	xi_deinit(state);
	program_state_deinit(state);
	emscripten_cancel_main_loop();
#endif
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

void xi_quit(xi_utils* xi){
	*xi->running = 0;
}

int main(int argc, char** argv){
	program_state state;
	program_state_init(&state);
	xi_init(&state);
	tick_reset(&state);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(do_frame_try, &state, 0, 1);
#else
	while (state.running){
		do_frame_try(&state);
	}
	xi_deinit(&state);
	program_state_deinit(&state);

#endif
	return 0;
}
