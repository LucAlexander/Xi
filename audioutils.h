#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <SDL2/SDL_mixer.h>
#include "hashMap.h"

HASHMAP(SoundMap, const char*, Mix_Chunk*)
HASHMAP(MusicMap, const char*, Mix_Music*)

typedef enum AUDIO_FLAGS{
	AUDIO_PLAYING=1,
	AUDIO_PAUSED=2
}AUDIO_FLAGS;

typedef struct AudioHandler{
	SoundMap sounds;
	MusicMap music;
	uint32_t audio_rate;
	uint32_t audio_channels;
	uint32_t audio_buffers;
	uint16_t audio_format;
	uint8_t flags;
	/* music playing
	 * music paused
	*/

}AudioHandler;

void audio_init(AudioHandler* ah);
void add_sound(AudioHandler* ah, const char* source, const char* name);
void add_music(AudioHandler* ah, const char* source, const char* name);
void play_sound(AudioHandler* ah, const char* name);
void play_music(AudioHandler* ah, const char* name);
uint8_t music_paused(AudioHandler* ah);
uint8_t music_playing(AudioHandler* ah);
void music_toggle_paused(AudioHandler* ah);
void music_set_paused(AudioHandler* ah, uint8_t p);
void audio_close(AudioHandler* ah);

#endif
