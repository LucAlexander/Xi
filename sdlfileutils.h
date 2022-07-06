#ifndef SDLFILEUTILS_H
#define SDLFILEUTILS_H

#include <SDL2/SDL.h>
#include "hashMap.h"

HASHMAP(TextureMap, const char*, SDL_Surface*)

typedef struct fileLoader{
	TextureMap textures;
}fileLoader;

void fileLoaderInit(fileLoader* loader);

SDL_Surface* loadImage(fileLoader* loader, const char* src);

void fileLoaderClose(fileLoader* loader);

#endif
