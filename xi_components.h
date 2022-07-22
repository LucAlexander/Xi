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
	ANIMATOR_C,/*animator_t*/\
	CLICKABLE_C,/*clickable_t*/\
	TEXT_C/*text_node_t*/

#define COMPONENT_SIZES\
	sizeof(v2),\
	sizeof(v2),\
	sizeof(Blitable),\
	sizeof(logic_t),\
	sizeof(repeater_t),\
	sizeof(animator_t),\
	sizeof(clickable_t),\
	sizeof(text_node_t)\
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

typedef struct clickable_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
	int32_t recharge_counter;
	int32_t recharge_time;
	uint8_t toggle;
	uint32_t w;
	uint32_t h;
}clickable_t;

void clickable_t_init(clickable_t* clicker, void f(SYSTEM_ARG_REQUIREMENTS), int32_t recharge_time, uint32_t w, uint32_t h);

typedef struct text_node_t{
	const char* text;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}text_node_t;

void text_node_t_init(text_node_t* node, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void text_node_set_text(text_node_t* node, const char* text);
void text_node_set_color(text_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif
