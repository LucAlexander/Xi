#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>

#include "hashMap.h"
#include "sdlfileutils.h"

struct v2;
struct v4;

enum RECT_DRAW_PARAMS{
	FILL=1,
	OUTLINE=2
};

typedef struct view{
	float x;
	float y;
	float px;
	float py;
	float pw;
	float ph;
}view;

HASHMAP(FontMap, const char*, TTF_Font*)

typedef struct fontHandler{
	char* activeFont;
	TTF_Font* fnt;
	FontMap list;
}fontHandler;

typedef enum RENDER_SCALE_QUALITY{
	RENDER_SCALE_NEAREST=0,
	RENDER_SCALE_LINEAR=1,
	RENDER_SCALE_ANISOTROPIC=2
}RENDER_SCALE_QUALITY;

typedef struct GraphicsHandler{
	SDL_Window* window;
	SDL_Renderer* renderer;
	view renderView;
	fontHandler fonts;
	uint16_t windowW;
	uint16_t windowH;
	float spriteScaleX;
	float spriteScaleY;
	RENDER_SCALE_QUALITY renderScale;
	fileLoader floader;
}GraphicsHandler;

void renderSetScaleQuality(GraphicsHandler* ghandle, RENDER_SCALE_QUALITY hint);
void renderSetBlendMode(GraphicsHandler* ghandle, SDL_BlendMode b);

void graphicsInit(GraphicsHandler* ghandle, uint16_t width, uint16_t height, const char* windowTitle);
void graphicsClose(GraphicsHandler* ghandle);

void handlerRenderViewInit(GraphicsHandler* ghandle);

SDL_Texture* getTexture(GraphicsHandler* ghandle, const char* src);

void renderSetView(GraphicsHandler* ghandle, view v);
view renderGetView(GraphicsHandler* ghandle);
void renderSetViewAbsolute(GraphicsHandler* ghandle);

void renderSetSpriteScale(GraphicsHandler* ghandle, float scaleX, float scaleY);
float scaleOnX(GraphicsHandler* ghandle, float val);
float scaleOnY(GraphicsHandler* ghandle, float val);

void toggleFullscreen(GraphicsHandler* ghandle);

struct v2 viewToWorldV2(GraphicsHandler* ghandle, struct v2 coords);
struct v2 worldToViewV2(GraphicsHandler* ghandle, struct v2 coords);
struct v2 viewToWorld(GraphicsHandler* ghandle, float x, float y);
struct v2 worldToView(GraphicsHandler* ghandle, float x, float y);

void viewToWorldV2Ptr(GraphicsHandler* ghandle, struct v2* coords);
void worldToViewV2Ptr(GraphicsHandler* ghandle, struct v2* coords);
void viewToWorldPtr(GraphicsHandler* ghandle, float* x, float* y);
void worldToViewPtr(GraphicsHandler* ghandle, float* x, float* y);

void renderFlip(GraphicsHandler* ghandle);
void renderClear(GraphicsHandler* ghandle);

void renderSetColor(GraphicsHandler* ghandle, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void renderSetTarget(GraphicsHandler* ghandle, SDL_Texture* t);

void formatDestRectToView(GraphicsHandler* ghandle, SDL_Rect* destRect);
void formatDestFRectToView(GraphicsHandler* ghandle, SDL_FRect* destRect);

typedef enum BLITABLE_FLAGS{
	BLITABLE_VISIBLE=1,
	BLITABLE_FLIP_H=2,
	BLITABLE_FLIP_V=4
}BLITABLE_FLAGS;

typedef struct Blitable{
	SDL_Texture* texture;
	SDL_Rect drawBound;
	uint32_t displayW;
	uint32_t displayH;
	int32_t textureW;
	int32_t textureH;
	BLITABLE_FLAGS flags;
	double angle;
	SDL_FPoint center;
}Blitable;

void BlitableInitF(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h);
void BlitableInit(Blitable* blit, SDL_Texture* t, uint32_t w, uint32_t h);
void renderBlitable(GraphicsHandler* ghandle, Blitable* blit, float x, float y);
void renderBlitableV2(GraphicsHandler* ghandle, Blitable* blit, struct v2 pos);
void BlitableFree(Blitable* blit);

void blitSurface(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect destRect);
void blitSurfaceEX(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect destRect, double angle, SDL_Point* center, SDL_RendererFlip flip);
void blitSurfaceF(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_FRect destRect);
void blitSurfaceEXF(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_FRect destRect, double angle, SDL_FPoint* center, SDL_RendererFlip flip);

void drawLineV2(GraphicsHandler* ghandle, struct v2, struct v2);
void drawLine(GraphicsHandler* ghandle, float x, float y, float xx, float yy);

// DIMENSION [X, Y, WIDTH, HEIGHT] DRAWING
void drawRectV2(GraphicsHandler* ghandle, struct v2, struct v2, uint8_t p);
void drawRectV4(GraphicsHandler* ghandle, struct v4, uint8_t p);
void drawRect(GraphicsHandler* ghandle, float x1, float y1, float x2, float y2, uint8_t p);
// BOUNDS [LEFT TOP RIGHT BOTTOM] DRAWING
void drawRectV2B(GraphicsHandler* ghandle, struct v2, struct v2, uint8_t p);
void drawRectV4B(GraphicsHandler* ghandle, struct v4, uint8_t p);
void drawRectB(GraphicsHandler* ghandle, float x1, float y1, float x2, float y2, uint8_t p);

void drawTextV2(GraphicsHandler* ghandle, struct v2 pos, const char* text);
void drawTextV2C(GraphicsHandler* ghandle, struct v2 pos, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void drawText(GraphicsHandler* ghandle, float x, float y, const char* text);
void drawTextC(GraphicsHandler* ghandle, float x, float y, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void drawTextEX(GraphicsHandler* ghandle, float x, float y, int32_t n, ...);

uint32_t getTextWidth(GraphicsHandler* ghandle, const char* c);
uint32_t getTextHeight(GraphicsHandler* ghandle, const char* c);
void queryTextSize(GraphicsHandler* ghandle, const char* text, int32_t* w, int32_t* h);

void fontHandlerInit(GraphicsHandler* ghandle);
void loadFont(GraphicsHandler* ghandle, const char* src, const char* name);
void setFont(GraphicsHandler* ghandle, char* fnt);
void fontHandlerClose(GraphicsHandler* ghandle);

#endif
