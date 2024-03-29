#define _GNU_SOURCE
#include "graphicsutils.h"
#include "mathutils.h"
#include "cflags.h"
#include <string.h>

#include <stdio.h>

HASHMAP_SOURCE(FontMap, const char*, TTF_Font*, hashS)
HASHMAP_SOURCE(Animations, const char*, animation_t, hashS)
VECTOR_SOURCE(vecT_t, SDL_Texture*)
MIN_PQ_SOURCE(renderq, renderq_entry_t)

void graphicsInit(GraphicsHandler* ghandle, uint16_t width, uint16_t height, const char* windowTitle){
	ghandle->renderScale = RENDER_SCALE_NEAREST;
	ghandle->window = NULL;
	ghandle->renderer = NULL;
	handlerRenderViewInit(ghandle);
	ghandle->windowW = 1920;
	ghandle->windowH = 1080;
	ghandle->spriteScaleX = 1;
	ghandle->spriteScaleY = 1;
	ghandle->texture_arena = vecT_tInit();
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	if (ghandle->window != NULL){
		SDL_DestroyWindow(ghandle->window);
	}
	if (ghandle->renderer != NULL){
		SDL_DestroyRenderer(ghandle->renderer);
	}
	SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &(ghandle->window), &(ghandle->renderer));
	SDL_SetWindowTitle(ghandle->window, windowTitle);
	view defaultView = {0, 0, 0, 0, width, height};
	ghandle->windowW = width;
	ghandle->windowH = height;
	renderSetView(ghandle, defaultView);
	fileLoaderInit(&ghandle->floader);
	fontHandlerInit(ghandle);
	ghandle->animations = AnimationsInit();
	ghandle->render_order = renderq_init();
	SDL_SetRenderDrawBlendMode(ghandle->renderer, SDL_BLENDMODE_BLEND);
}

void graphicsClose(GraphicsHandler* ghandle){
	renderq_free(&ghandle->render_order);
	AnimationsFree(&ghandle->animations);
	texture_arena_release(ghandle);
	vecT_tFree(&ghandle->texture_arena);
	fontHandlerClose(ghandle);
	fileLoaderClose(&ghandle->floader);
	TTF_Quit();
	SDL_DestroyWindow(ghandle->window);
	SDL_DestroyRenderer(ghandle->renderer);
	ghandle->window = NULL;
	ghandle->renderer = NULL;
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void handlerRenderViewInit(GraphicsHandler* ghandle){
	ghandle->renderView.x = 0;
	ghandle->renderView.y = 0;
	ghandle->renderView.px = 0;
	ghandle->renderView.py = 0;
	ghandle->renderView.pw = 0;
	ghandle->renderView.ph = 0;
}

SDL_Texture* getTexture(GraphicsHandler* ghandle, const char* src){
	SDL_Texture* item = SDL_CreateTextureFromSurface(ghandle->renderer, loadImage(&ghandle->floader, src));
	if (item == NULL){
		printf("[!] Unable to construct image from file \'%s\'\n%s\n",src, SDL_GetError());
	}
	return item;
}

void renderSetSpriteScale(GraphicsHandler* ghandle, float scaleX, float scaleY){
	ghandle->spriteScaleX = scaleX;
	ghandle->spriteScaleY = scaleY;
	SDL_RenderSetScale(ghandle->renderer, ghandle->spriteScaleX, ghandle->spriteScaleY);
}

void renderSetBlendMode(GraphicsHandler* ghandle, SDL_BlendMode b){
	SDL_SetRenderDrawBlendMode(ghandle->renderer, b);
}

float scaleOnX(GraphicsHandler* ghandle, float val){
	return val/ghandle->spriteScaleX;
}

float scaleOnY(GraphicsHandler* ghandle, float val){
	return val/ghandle->spriteScaleY;
}

uint8_t render_in_view(GraphicsHandler* ghandle, float x, float y, float w, float h){
	v4 rect = {x, y, w, h};
	return render_in_view_v4(ghandle, rect);
}

uint8_t render_in_view_v4(GraphicsHandler* ghandle, v4 rect){
	v4 v = {
		ghandle->renderView.px,
		ghandle->renderView.py,
		ghandle->renderView.pw,
		ghandle->renderView.ph,
	};
	return rectCollides(v, rect);
}

void renderSetView(GraphicsHandler* ghandle, view v){
	ghandle->renderView = v;
	const SDL_Rect port = {v.px, v.py, v.pw, v.ph};
	SDL_RenderSetViewport(ghandle->renderer, &port);
	SDL_RenderSetLogicalSize(ghandle->renderer, v.pw, v.ph);
}

view renderGetView(GraphicsHandler* ghandle){
	return ghandle->renderView;
}

void renderSetViewAbsolute(GraphicsHandler* ghandle){
	view absview = {
		0, 0,
	       	0, 0,
	       	ghandle->windowW, ghandle->windowH
	};
	renderSetView(ghandle, absview);
}

void renderSetScaleQuality(GraphicsHandler* ghandle, RENDER_SCALE_QUALITY hint){
	ghandle->renderScale = hint;
}

void toggleFullscreen(GraphicsHandler* ghandle){
	uint32_t flags = (SDL_GetWindowFlags(ghandle->window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (SDL_SetWindowFullscreen(ghandle->window, flags) < 0){
		printf("[!] Toggling fullscreen mode failed\n");
		return;
	}
	renderSetView(ghandle, ghandle->renderView);
	if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0){
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, (const char*)ghandle->renderScale);
	}
}

v2 viewToWorldV2(GraphicsHandler* ghandle, v2 coords){
	coords.x += ghandle->renderView.x;
	coords.y += ghandle->renderView.y;
	return coords;
}

v2 worldToViewV2(GraphicsHandler* ghandle, v2 coords){
	coords.x -= ghandle->renderView.x;
	coords.y -= ghandle->renderView.y;
	return coords;
}

v2 viewToWorld(GraphicsHandler* ghandle, float x, float y){
	v2 a = {x,y};
	return viewToWorldV2(ghandle, a);
}

v2 worldToView(GraphicsHandler* ghandle, float x, float y){
	v2 a = {x,y};
	return worldToViewV2(ghandle, a);
}

void viewToWorldV2Ptr(GraphicsHandler* ghandle, struct v2* coords){
	coords->x += ghandle->renderView.x;
	coords->y += ghandle->renderView.y;
}

void worldToViewV2Ptr(GraphicsHandler* ghandle, struct v2* coords){
	coords->x -= ghandle->renderView.x;
	coords->y -= ghandle->renderView.y;
}

void viewToWorldPtr(GraphicsHandler* ghandle, float* x, float* y){
	if (x!=NULL){
		*x += ghandle->renderView.x;
	}
	if (y!=NULL){
		*y += ghandle->renderView.y;
	}
}

void worldToViewPtr(GraphicsHandler* ghandle, float* x, float* y){
	if (x!=NULL){
		*x -= ghandle->renderView.x;
	}
	if (y!=NULL){
		*y -= ghandle->renderView.y;
	}
}

void renderFlip(GraphicsHandler* ghandle){
	SDL_RenderPresent(ghandle->renderer);
}

void renderClear(GraphicsHandler* ghandle){
	SDL_RenderClear(ghandle->renderer);
}

void renderSetColor(GraphicsHandler* ghandle, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	SDL_SetRenderDrawColor(ghandle->renderer, r, g, b, a);
}

void renderSetTarget(GraphicsHandler* ghandle, SDL_Texture* t){
	SDL_SetRenderTarget(ghandle->renderer, t);
}

void formatDestRectToView(GraphicsHandler* ghandle, SDL_Rect* destRect){
	destRect->x-=ghandle->renderView.x;
	destRect->y-=ghandle->renderView.y;
}

void formatDestFRectToView(GraphicsHandler* ghandle, SDL_FRect* destRect){
	destRect->x-=ghandle->renderView.x;
	destRect->y-=ghandle->renderView.y;
}

void BlitableInitF_arena(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h){
	SDL_Texture* t = getTexture(ghandle, source);
	vecT_tPushBack(&ghandle->texture_arena, t);
	BlitableInit(blit, t, w, h);
}

void BlitableInitF(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h){
	BlitableInit(blit, getTexture(ghandle, source), w, h);
}

void BlitableInit(Blitable* blit, SDL_Texture* t, uint32_t w, uint32_t h){
	blit->texture = t;
	SDL_Rect bound = {0, 0, w, h};
	blit->drawBound = bound;
	blit->displayW = w;
	blit->displayH = h;
	SDL_QueryTexture(t, NULL, NULL, &(blit->textureW), &(blit->textureH));
	blit->flags = BLITABLE_VISIBLE;
	blit->angle = 0;
	SDL_FPoint mid = {w/2, h/2};
	blit->center = mid;
}

void renderBlitable(GraphicsHandler* ghandle, Blitable* blit, float x, float y){
	if (!(blit->flags & BLITABLE_VISIBLE)){
		return;
	}
	SDL_RendererFlip flipArg = SDL_FLIP_NONE;
	if (blit->flags & BLITABLE_FLIP_H){
		flipArg |= SDL_FLIP_HORIZONTAL;
		flipArg &= ~(SDL_FLIP_NONE);
	}
	if (blit->flags & BLITABLE_FLIP_V){
		flipArg |= SDL_FLIP_VERTICAL;
		flipArg &= ~(SDL_FLIP_NONE);
	}
	SDL_FRect dest = {x-blit->center.x, y-blit->center.y, blit->displayW, blit->displayH};
	blitSurfaceEXF(ghandle, blit->texture, &(blit->drawBound), dest, blit->angle, &(blit->center), flipArg);
}

void renderBlitableV2(GraphicsHandler* ghandle, Blitable* blit, v2 pos){
	renderBlitable(ghandle, blit, pos.x, pos.y);
}

void BlitableFree(Blitable* blit){
	SDL_DestroyTexture(blit->texture);
	blit->texture = NULL;
}

void animator_t_init(animator_t* animator){
	animator->name = "";
	animator->pos_x = 0;
	animator->pos_y = 0;
	animator->bounds_x = 0;
	animator->bounds_y = 0;
	animator->index = 0;
	animator->flags = 1;
	animator->frame_time = 0;
	animator->frame_time_counter = 0;
}

void animator_set_animation(GraphicsHandler* ghandle, animator_t* animator, const char* name, Blitable* sprite){
	if (animator->name == name){
		return;
	}
	animator->name = name;
	animator->bounds_x = sprite->textureW/sprite->drawBound.w;
	animator->bounds_y = sprite->textureH/sprite->drawBound.h;
	animator->index = -1;
	animator->frame_time_counter = 0;
	progress_animation(ghandle, animator);
}

void animator_set_loop(animator_t* animator, uint8_t loop){
	animator->flags = bit_set(animator->flags, 2, loop);
}

void animator_set_active(animator_t* animator, uint8_t active){
	animator->flags = bit_set(animator->flags, 1, active);
}

uint8_t animator_get_loop(animator_t* animator){
	return bit_on(animator->flags, 2);
}

uint8_t animator_get_active(animator_t* animator){
	return bit_on(animator->flags, 1);
}

void animator_set_frame_time(animator_t* animator, uint32_t ft){
	animator->frame_time = ft;
}

uint32_t animator_get_frame_time(animator_t* animator){
	return animator->frame_time;
}

void add_animation(GraphicsHandler* ghandle, const char* name, uint32_t start_frame_col, uint32_t start_frame_row, uint32_t length){
	animation_t anim = {start_frame_col, start_frame_row, length};
	AnimationsPush(&ghandle->animations, name, anim);
}

void progress_animation(GraphicsHandler* ghandle, animator_t* animator){
	if (!bit_check(animator->flags, 1)){
		return;
	}
	animation_t animation = AnimationsGet(&ghandle->animations, animator->name).val;
	if (animator->index == -1){
		animator->pos_x = animation.x;
		animator->pos_y = animation.y;
		animator->index++;
		return;
	}
	if (animator->index == animation.z-1){
		if (!bit_check(animator->flags, 2)){
			return;
		}
		animator->index = 0;
		animator->pos_x = animation.x;
		animator->pos_y = animation.y;
		return;
	}
	animator->pos_x++;
	animator->index++;
	if (animator->pos_x >= animator->bounds_x){
		animator->pos_y ++;
		animator->pos_y %= animator->bounds_y;
		animator->pos_x = 0;
	}
}

void blitSurface(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect destRect){
	if (!render_in_view(ghandle, destRect.x, destRect.y, destRect.w, destRect.h)){
		return;
	}
	formatDestRectToView(ghandle, &destRect);
	SDL_RenderCopy(ghandle->renderer, texture, srcRect, &destRect);
}

void blitSurfaceEX(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect destRect, double angle, SDL_Point* center, SDL_RendererFlip flip){
	if (!render_in_view(ghandle, destRect.x, destRect.y, destRect.w, destRect.h)){
		return;
	}
	formatDestRectToView(ghandle, &destRect);
	SDL_RenderCopyEx(ghandle->renderer, texture, srcRect, &destRect, angle, center, flip);
}

void blitSurfaceF(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_FRect destRect){
	if (!render_in_view(ghandle, destRect.x, destRect.y, destRect.w, destRect.h)){
		return;
	}
	formatDestFRectToView(ghandle, &destRect);

	SDL_RenderCopyF(ghandle->renderer, texture, srcRect, &destRect);
}

void blitSurfaceEXF(GraphicsHandler* ghandle, SDL_Texture* texture, SDL_Rect* srcRect, SDL_FRect destRect, double angle, SDL_FPoint* center, SDL_RendererFlip flip){
	if (!render_in_view(ghandle, destRect.x, destRect.y, destRect.w, destRect.h)){
		return;
	}
	formatDestFRectToView(ghandle, &destRect);
	SDL_RenderCopyExF(ghandle->renderer, texture, srcRect, &destRect, angle, center, flip);
}

void drawLineV2(GraphicsHandler* ghandle, v2 a, v2 b){
	drawLine(ghandle, a.x, a.y, b.x, b.y);
}

void drawLine(GraphicsHandler* ghandle, float x, float y, float xx, float yy){
	SDL_RenderDrawLine(ghandle->renderer, x-ghandle->renderView.x, y-ghandle->renderView.y, xx-ghandle->renderView.x, yy-ghandle->renderView.y);
}

void drawRectV2(GraphicsHandler* ghandle, v2 a, v2 b, uint8_t p){
	drawRect(ghandle, a.x, a.y, b.x, b.y, p);
}

void drawRectV4(GraphicsHandler* ghandle, v4 r, uint8_t p){
	drawRect(ghandle, r.x, r.y, r.w, r.h, p);
}

void drawRect(GraphicsHandler* ghandle, float x1, float y1, float x2, float y2, uint8_t p){
	SDL_FRect bound = {
		x1-ghandle->renderView.x,
		y1-ghandle->renderView.y,
	       	x2,
	       	y2
	};
	if (p & FILL){
		SDL_RenderFillRectF(ghandle->renderer, &bound);
	}
	if (p & OUTLINE){
		SDL_RenderDrawRectF(ghandle->renderer, &bound);
	}
}

void drawRectV2B(GraphicsHandler* ghandle, v2 a, v2 b, uint8_t p){
	drawRectB(ghandle, a.x, a.y, b.x, b.y, p);
}

void drawRectV4B(GraphicsHandler* ghandle, v4 r, uint8_t p){
	drawRectB(ghandle, r.x, r.y, r.w, r.h, p);
}

void drawRectB(GraphicsHandler* ghandle, float x1, float y1, float x2, float y2, uint8_t p){
	drawRect(
		ghandle, 
		x1,
		y1, 
		x2-x1-ghandle->renderView.x,
		y2-y1-ghandle->renderView.y,
	p);
}

void fontHandlerInit(GraphicsHandler* ghandle){
	ghandle->fonts.activeFont = "";
	ghandle->fonts.fnt = NULL;
	ghandle->fonts.list = FontMapInit();
}

void loadFont(GraphicsHandler* ghandle, const char* src, const char* name){
	TTF_Font* f = FontMapGet(&(ghandle->fonts.list), name).val;
	if (f != NULL){
		printf("[!] font with name \'%s\' already loaded\n%s\n", src, SDL_GetError());
		return;
	}
	f = TTF_OpenFont(src, 16);
	if (f == NULL){
		TTF_CloseFont(f);
		f = NULL;
		printf("[!] Unable to load font \'%s\'\n%s\n", src, SDL_GetError());
		return;
	}
	FontMapPush(&(ghandle->fonts.list), name, f);
}

void setFont(GraphicsHandler* ghandle, char* name){
	TTF_Font* temp = FontMapGet(&(ghandle->fonts.list), name).val;
	if (temp == NULL){
		printf("[!] No font %s is loaded, and cannot be set to active font\n",name);
		return;
	}
	ghandle->fonts.activeFont = name;
	ghandle->fonts.fnt = temp;
}

void fontHandlerClose(GraphicsHandler* ghandle){
	FontMapIterator it = FontMapIteratorInit(&(ghandle->fonts.list));
	while(FontMapIteratorHasNext(&it)){
		TTF_CloseFont(FontMapGet(&(ghandle->fonts.list), FontMapIteratorNext(&it).key).val);
	}
	FontMapFree(&(ghandle->fonts.list));
}

void drawTextV2(GraphicsHandler* ghandle, v2 pos, const char* text){
	drawText(ghandle, pos.x, pos.y, text);
}

void drawTextV2C(GraphicsHandler* ghandle, v2 pos, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	drawTextC(ghandle, pos.x, pos.y, text, r, g, b, a);
}

void drawText(GraphicsHandler* ghandle, float x, float y, const char* text){
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(ghandle->renderer, &r, &g, &b, &a);
	drawTextC(ghandle, x, y, text, r, g, b, a);
}

void drawTextC(GraphicsHandler* ghandle, float x, float y, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	if (text==NULL){
		return;
	}
	SDL_Color c = {r, g, b};
	SDL_Surface* surf = TTF_RenderText_Solid(ghandle->fonts.fnt, text, c);
	SDL_Texture* t = SDL_CreateTextureFromSurface(ghandle->renderer, surf);
	SDL_SetTextureAlphaMod(t, a);
	int w, h;
	TTF_SizeText(ghandle->fonts.fnt, text, &w, &h);
	SDL_Rect dest = {x, y, w, h};
	blitSurface(ghandle, t, NULL, dest);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(t);
}

void drawTextEX(GraphicsHandler* ghandle, float x, float y, int32_t n, ...){
	va_list args;
	va_start(args, n);
	const char* text;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
	uint32_t width = 0;
	while(n > 0){
		text = va_arg(args, const char*);
		red = va_arg(args,int);
		green = va_arg(args,int);
		blue = va_arg(args,int);
		alpha = va_arg(args,int);
		n--;
		drawTextC(ghandle, x+width, y, text, red, green, blue, alpha);
		width += getTextWidth(ghandle, text);
	}
	va_end(args);
}

uint32_t getTextWidth(GraphicsHandler* ghandle, const char* c){
	int32_t w;
	TTF_SizeText(ghandle->fonts.fnt, c, &w, NULL);
	return w;
}

uint32_t getTextHeight(GraphicsHandler* ghandle, const char* c){
	int32_t h;
	TTF_SizeText(ghandle->fonts.fnt, c, &h, NULL);
	return h;
}

void queryTextSize(GraphicsHandler* ghandle, const char* text, int32_t* w, int32_t* h){
	TTF_SizeText(ghandle->fonts.fnt, text, w, h);
}

void setFontKerning(GraphicsHandler* ghandle, uint32_t kerning){
	if (ghandle->fonts.fnt == NULL) return;
	TTF_SetFontKerning(ghandle->fonts.fnt, kerning);
}

uint32_t getFontKerning(GraphicsHandler* ghandle){
	if (ghandle->fonts.fnt == NULL) return 0;
	return TTF_GetFontKerning(ghandle->fonts.fnt);
}

void texture_arena_release(GraphicsHandler* ghandle){
	uint32_t i;
	for (i = 0;i<ghandle->texture_arena.size;++i){
		SDL_DestroyTexture(ghandle->texture_arena.data[i]);
	}
}
