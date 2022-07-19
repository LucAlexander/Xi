#ifndef XI_COMPONENTS_H
#define XI_COMPONENTS_H

#include "systems.h"
#include "config/project-components.h"

#define STD_COMPONENTS\
	POSITION_C,/*v2*/\
	FORCES_C,/*v2*/\
	BLITABLE_C,/*Blitable*/\
	BEHAVIOR_C,/*logic_t*/\
	REPEATER_C,/*repeater_t*/\
	ANIMATOR_C/*animator_t*/

#define COMPONENT_SIZES\
	sizeof(v2),\
	sizeof(v2),\
	sizeof(Blitable),\
	sizeof(logic_t),\
	sizeof(repeater_t),\
	sizeof(animator_t)\
	USER_COMPONENT_SIZES

typedef enum COMPONENTS{
	STD_COMPONENTS,
	USER_COMPONENTS
}COMPONENTS;

typedef struct logic_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
}logic_t;

typedef struct repeater_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
	uint32_t ticks;
	uint32_t trigger_time;
	uint32_t trigger_count;
	uint8_t destroy_after;
}repeater_t;

void repeater_t_init(repeater_t* wrapper, void f(SYSTEM_ARG_REQUIREMENTS), uint32_t interval_time, uint32_t count, uint8_t destroy);

#endif
