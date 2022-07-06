#include <stdio.h>

#include "graphicsutils.h"
#include "inpututils.h"
#include "mathutils.h"

int main(int argc, char** argv){
	GraphicsHandler graphics;
	graphicsInit(&graphics, 500, 500, "Layered Rendering Test");
	renderSetBlendMode(&graphics, SDL_BLENDMODE_BLEND);

	input inp;
	inputInit(&inp);
	view v =  {0, 0, 0, 0, 500, 500};
	renderSetView(&graphics, v);
	SDL_Event event;
	uint8_t run = 1;


	v2 a = {25, 25};
	v2 b = {50, 50};


	v2 c = {35, 35};
	v2 d = {60, 60};


	while (run){
		while (SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					run = 0;
				break;
			}
		}
		// RENDERING
		newInputFrame(&inp);
		renderSetColor(&graphics, 0, 0, 0, 255);
		renderClear(&graphics);
		//
		renderSetColor(&graphics, 128, 0, 0, 255);
		drawRectV2(&graphics, a, b, FILL);
		renderSetColor(&graphics, 0, 128, 128, 128);
		drawRectV2(&graphics, c, d, FILL);
		//
		renderFlip(&graphics);

	}
	graphicsClose(&graphics);
	return 0;
}
