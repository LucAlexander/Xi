#ifndef XI_CONFIG_H
#define XI_CONFIG_H

#define WINDOW_W 384
#define WINDOW_H 216
#define WINDOW_TITLE "XI"

#define MEM_POOL_INITIAL 1024*4096
#define MEM_POOL_RESIZE 1024*1024
#define MEM_POOL_RESIZE_COUNT 1

#define SECOND 1000
#define H_SECOND SECOND/2
#define Q_SECOND SECOND/4

#ifdef __EMSCRIPTEN__
#define XI_FONT "./fnt/"
#define XI_SPRITE "./spr/"
#define XI_SOUND "./snd/"
#else
#define XI_FONT "../fnt/"
#define XI_SPRITE "../spr/"
#define XI_SOUND "../snd/"
#endif

#endif
