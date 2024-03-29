#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>

#include "hashMap.h"
#include "vector.h"
#include "minpq.h"

#include "sdlfileutils.h"
#include "systems.h"

#define RENDER_GUI_DEPTH 10000

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

typedef struct animation_t{
	int32_t x, y, z;
}animation_t;

typedef struct renderq_entry_t{
	SYSTEM_ARG_DATA;
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
}renderq_entry_t;

HASHMAP(FontMap, const char*, TTF_Font*)
HASHMAP(Animations, const char*, animation_t)
VECTOR(vecT_t, SDL_Texture*)
MIN_PQ(renderq, renderq_entry_t)

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
	vecT_t texture_arena;
	Animations animations;
	renderq_t render_order;
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

uint8_t render_in_view(GraphicsHandler* ghandle, float x, float y, float x2, float y2);
uint8_t render_in_view_v4(GraphicsHandler* ghandle, struct v4 rect);

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
void BlitableInitF_arena(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h);
void BlitableInit(Blitable* blit, SDL_Texture* t, uint32_t w, uint32_t h);
void renderBlitable(GraphicsHandler* ghandle, Blitable* blit, float x, float y);
void renderBlitableV2(GraphicsHandler* ghandle, Blitable* blit, struct v2 pos);
void BlitableFree(Blitable* blit);

typedef struct animator_t{
	const char* name;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t bounds_x;
	uint32_t bounds_y;
	int32_t index;
	/* active
	 * loop
	 */
	uint8_t flags;
	uint32_t frame_time;
	uint32_t frame_time_counter;
}animator_t;

void animator_t_init(animator_t* animator);
void animator_set_animation(GraphicsHandler* ghandle, animator_t* animator, const char* name, Blitable* sprite);
void animator_set_loop(animator_t* animator, uint8_t loop);
void animator_set_active(animator_t* animator, uint8_t active);
uint8_t animator_get_loop(animator_t* animator);
uint8_t animator_get_active(animator_t* animator);
void animator_set_frame_time(animator_t* animator, uint32_t ft);
uint32_t animator_get_frame_time(animator_t* animator);

void add_animation(GraphicsHandler* ghandle, const char* name, uint32_t start_frame_col, uint32_t start_frame_row, uint32_t length);
void progress_animation(GraphicsHandler* ghandle, animator_t* animator);

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

void setFontKerning(GraphicsHandler* ghandle, uint32_t kerning);
uint32_t getFontKerning(GraphicsHandler* ghandle);

void fontHandlerInit(GraphicsHandler* ghandle);
void loadFont(GraphicsHandler* ghandle, const char* src, const char* name);
void setFont(GraphicsHandler* ghandle, char* fnt);
void fontHandlerClose(GraphicsHandler* ghandle);
void texture_arena_release(GraphicsHandler* ghandle);

#endif
