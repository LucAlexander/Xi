#include "xi_components.h"

void repeater_t_init(repeater_t* wrapper, void f(SYSTEM_ARG_REQUIREMENTS), uint32_t interval_time, uint32_t count, uint8_t destruct){
	wrapper->f = f;
	wrapper->ticks = 0;
	wrapper->trigger_time = interval_time;
	wrapper->trigger_count = count;
	wrapper->destroy_after = destruct;
}

void clickable_t_init(clickable_t* clicker, void f(SYSTEM_ARG_REQUIREMENTS), int32_t recharge_time, uint32_t w, uint32_t h){
	clicker->f = f;
	clicker->recharge_counter = 0;
	clicker->recharge_time = recharge_time;
	clicker->toggle = 0;
	clicker->w = w;
	clicker->h = h;
}

void text_node_t_init(text_node_t* node, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	node->text = text;
	node->red = r;
	node->green = g;
	node->blue = b;
	node->alpha = a;
}

void text_node_set_text(text_node_t* node, const char* text){
	node->text = text;
}

void text_node_set_color(text_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	node->red = r;
	node->green = g;
	node->blue = b;
	node->alpha = a;
}

