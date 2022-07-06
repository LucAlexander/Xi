#include "inpututils.h"
#include "mathutils.h"
#include "graphicsutils.h"

#include <string.h>

void inputInit(input* inp){
	memset(inp->heldKeys, 0, 256);
	memset(inp->pressedKeys, 0, 256);
	memset(inp->releasedKeys, 0, 256);
	memset(inp->heldMouseButtons, 0, 5);
	memset(inp->pressedMouseButtons, 0, 5);
	memset(inp->releasedMouseButtons, 0, 5);
	inp->scrollDir = 0;
}

void newInputFrame(input* inp){
	memset(inp->pressedKeys, 0, 256);
	memset(inp->releasedKeys, 0, 256);
	memset(inp->pressedMouseButtons, 0, 5);
	memset(inp->releasedMouseButtons, 0, 5);
	inp->scrollDir = 0;
}

void mouseScrollEvent(input* inp, int8_t dir){
	inp->scrollDir = dir;
}

void mouseMoveEvent(input* inp, int32_t x, int32_t y){
	inp->mouseX = x;
	inp->mouseY = y;
}

int32_t mouseX(input* inp){
	return inp->mouseX;
}

int32_t mouseY(input* inp){
	return inp->mouseY;
}

v2 mousePos(input* inp){
	v2 a = {mouseX(inp), mouseY(inp)};
	return a;
}

void mouseUpEvent(input* inp, const SDL_Event event){
	inp->releasedMouseButtons[event.button.button-1] = 1;
	inp->heldMouseButtons[event.button.button-1] = 0;
}

void mouseDownEvent(input* inp, const SDL_Event event){
	inp->pressedMouseButtons[event.button.button-1] = 1;
	inp->heldMouseButtons[event.button.button-1] = 1;
}

void keyUpEvent(input* inp, const SDL_Event event){
	inp->releasedKeys[event.key.keysym.scancode] = 1;
	inp->heldKeys[event.key.keysym.scancode] = 0;
}

void keyDownEvent(input* inp, const SDL_Event event){
	inp->pressedKeys[event.key.keysym.scancode] = 1;
	inp->heldKeys[event.key.keysym.scancode] = 1;
}

uint8_t mouseScrolled(input* inp, int8_t dir){
	return sign(dir)==sign(inp->scrollDir);
}

uint8_t mouseHeld(input* inp, uint8_t button){
	return inp->heldMouseButtons[button-1];
}

uint8_t mousePressed(input* inp, uint8_t button){
	return inp->pressedMouseButtons[button-1];
}

uint8_t mouseReleased(input* inp, uint8_t button){
	return inp->releasedMouseButtons[button-1];
}

uint8_t keyHeld(input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->heldKeys[k];
}

uint8_t keyPressed(input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->pressedKeys[k];
}

uint8_t keyReleased(input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	return inp->releasedKeys[k];
}

void mouseInterrupt(input* inp, uint8_t b){
	inp->pressedMouseButtons[b-1] = 0;
	inp->heldMouseButtons[b-1] = 0;
}

void keyInterrupt(input* inp, const char* key){
	SDL_Scancode k = SDL_GetScancodeFromName(key);
	inp->pressedKeys[k] = 0;
	inp->heldKeys[k] = 0;
}
