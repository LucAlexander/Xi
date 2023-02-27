#ifndef XI_H
#define XI_H

#include "graphicsutils.h"
#include "audioutils.h"
#include "inpututils.h"
#include "entities.h"
#include "vector.h"
#include "collision_mask.h"
#include "systems.h"

#include <inttypes.h>

#define SRC 0

struct system_t;
struct project_structs;

VECTOR(vsys_t, struct system_t)
HASHMAP(mu32u8_t, uint32_t, uint8_t)

typedef enum PROGRAM_STATE{
	XI_STATE_UPDATE_PRE,
	XI_STATE_UPDATE,
	XI_STATE_UPDATE_POST,
	XI_STATE_RENDER,
	XI_SYSTEM_STATE_COUNT,
	XI_STATE_INIT,
	XI_STATE_DEINIT
}PROGRAM_STATE;

typedef struct program_state{
	uint8_t running;
	uint32_t base_time;
	uint32_t tick;
	vsys_t system_group[XI_SYSTEM_STATE_COUNT];
	PROGRAM_STATE state;
	SDL_Event event;
	GraphicsHandler graphics;
	AudioHandler audio;
	input user_input;
	entity_data ecs;
	spacial_quadtree_node_t* colliders;
	struct project_structs* project;
	uint8_t debug;
}program_state;

typedef struct xi_utils{
	GraphicsHandler* graphics;
	AudioHandler* audio;
	input* user_input;
	entity_data* ecs;
	spacial_quadtree_node_t* colliders;
	struct project_structs* project;
	uint32_t ticks;
	uint8_t* debug;
}xi_utils;

void program_state_init(program_state* state);
void program_state_deinit(program_state* state);

void reset_arena(xi_utils* xi);

void xi_init(program_state* state);
void xi_deinit(program_state* state);
void xi_run_system_group(program_state* state, uint32_t group);
void xi_run_system_group_queued(program_state* state, uint32_t group);

void xi_entities_spawn(xi_utils* xi);
void xi_persistent(SYSTEM_ARG_REQUIREMENTS);

void tick_reset(program_state* state);

void do_frame_try(program_state* state);
void read_user_input(program_state* state);

xi_utils construct_xi_utils(program_state*);

void std_systems(program_state* state);
void system_add(program_state* state, struct system_t, PROGRAM_STATE);

void xisetup(program_state*, xi_utils* xi);
void xistart(xi_utils*);

#endif
