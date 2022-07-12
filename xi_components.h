#ifndef XI_COMPONENTS_H
#define XI_COMPONENTS_H

#include "systems.h"

#define STD_COMPONENTS\
	POSITION_C,/*v2*/\
	FORCES_C,/*v2*/\
	BLITABLE_C,/*Blitable*/\
	BEHAVIOR_C,/*void (*f)(SYSTEM_ARG_REQUIREMENTS)*/\
	REPEATER_C/**/

typedef enum STD_COMPONENTS_MOC{
	POSITION_C_MOC,
	FORCES_C_MOC,
	BLITABLE_C_MOC,
	BEHAVIOR_C_MOC,
	REPEATER_C_MOC
}STD_COMPONENTS_MOC;

typedef struct repeater_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
	uint32_t ticks;
	uint32_t trigger_time;
	uint32_t trigger_count;
	uint8_t destroy_after;
}repeater_t;

void repeater_t_init(repeater_t* wrapper, void f(SYSTEM_ARG_REQUIREMENTS), uint32_t interval_time, uint32_t count, uint8_t destroy);

#endif
