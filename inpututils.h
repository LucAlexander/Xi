#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <SDL2/SDL.h>

#include <inttypes.h>

struct v2;

typedef struct input{
	uint8_t heldKeys[256];
	uint8_t pressedKeys[256];
	uint8_t releasedKeys[256];
	uint8_t heldMouseButtons[5];
	uint8_t pressedMouseButtons[5];
	uint8_t releasedMouseButtons[5];
	int8_t scrollDir;
	int32_t mouseX;
	int32_t mouseY;
}input;

void inputInit(input* inp);

void newInputFrame(input* inp);

void mouseMoveEvent(input* inp, int32_t x, int32_t y);

int32_t mouseX(input* inp);
int32_t mouseY(input* inp);
struct v2 mousePos(input* inp);

void mouseScrollEvent(input* inp, int8_t);

void mouseUpEvent(input* inp, const SDL_Event event);
void mouseDownEvent(input* inp, const SDL_Event event);

void keyUpEvent(input* inp, const SDL_Event event);
void keyDownEvent(input* inp, const SDL_Event event);

uint8_t mouseScrolled(input* inp, int8_t);

uint8_t mouseHeld(input* inp, uint8_t);
uint8_t mousePressed(input* inp, uint8_t);
uint8_t mouseReleased(input* inp, uint8_t);

uint8_t keyHeld(input* inp, const char*);
uint8_t keyPressed(input* inp, const char*);
uint8_t keyReleased(input* inp, const char*);

void mouseInterrupt(input* inp, uint8_t);
void keyInterrupt(input* inp, const char*);

#endif
