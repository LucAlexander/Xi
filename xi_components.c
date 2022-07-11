#include "xi_components.h"

void repeater_t_init(repeater_t* wrapper, void f(SYSTEM_ARG_REQUIREMENTS), uint32_t interval_time, uint32_t count){
	wrapper->f = f;
	wrapper->ticks = 0;
	wrapper->trigger_time = interval_time;
	wrapper->trigger_count = count;
}
