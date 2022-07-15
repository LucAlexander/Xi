#include "audioutils.h"
#include "cflags.h"

HASHMAP_SOURCE(SoundMap, const char*, Mix_Chunk*, hashS)
HASHMAP_SOURCE(MusicMap, const char*, Mix_Music*, hashS)

void audio_init(AudioHandler* ah){
	ah->audio_rate = 22050;
	ah->audio_format = AUDIO_S16SYS;
	ah->audio_channels = 2;
	ah->audio_buffers = 4096;
	ah->flags = 0;
	ah->sounds = SoundMapInit();
	ah->music = MusicMapInit();
	if (Mix_OpenAudio(ah->audio_rate, ah->audio_format, ah->audio_channels, ah->audio_buffers)!=0){
		printf("[!] Unable to open audio channel\n\taudio_rate : %u\n\taudio_format : %u\n\taudio_channels : %u\n\taudio_buffers : %u\n%s\n", ah->audio_rate, ah->audio_format, ah->audio_channels, ah->audio_buffers, SDL_GetError());
	}
}

void add_sound(AudioHandler* ah, const char* source, const char* name){
	Mix_Chunk* sound = Mix_LoadWAV(source);
	if (!sound){
		printf("[!] Unable to open audio file \'%s\'\n%s\n", source, SDL_GetError());
		return;
	}
	Mix_Chunk* prev = SoundMapPop(&ah->sounds, name).val;
	Mix_FreeChunk(prev);
	prev = NULL;
	SoundMapPush(&ah->sounds, name, sound);
}

void add_music(AudioHandler* ah, const char* source, const char* name){
	Mix_Music* song = Mix_LoadMUS(source);
	if (!song){
		printf("[!] Unable to open audio file \'%s\'\n%s\n", source, SDL_GetError());
		return;
	}
	Mix_Music* prev = MusicMapPop(&ah->music, name).val;
	Mix_FreeMusic(prev);
	prev = NULL;
	MusicMapPush(&ah->music, name, song);
}

void play_sound(AudioHandler* ah, const char* name){
	Mix_Chunk* sound = SoundMapGet(&ah->sounds, name).val;
	if (!sound){
		return;
	}
	if (Mix_PlayChannel(-1, sound, 1)==-1){
		printf("[!] Unable to play sound \'%s\'\n%s\n", name, Mix_GetError());
	}
}

void play_music(AudioHandler* ah, const char* name){
	Mix_Music* song = MusicMapGet(&ah->music, name).val;
	if (!song){
		return;
	}
	if (Mix_PlayMusic(song, -1)==-1){
		printf("[!] Unable to play song \'%s\'\n%s\n", name, Mix_GetError());
		return;
	}
	ah->flags = bit_on(ah->flags, AUDIO_PLAYING);
}

uint8_t music_paused(AudioHandler* ah){
	return bit_check(ah->flags, AUDIO_PAUSED);
}

uint8_t music_playing(AudioHandler* ah){
	return bit_check(ah->flags, AUDIO_PLAYING);
}

void music_toggle_paused(AudioHandler* ah){
	if (music_paused(ah)){
		Mix_ResumeMusic();
		ah->flags = bit_off(ah->flags, AUDIO_PAUSED);
		return;
	}
	Mix_PauseMusic();
	ah->flags = bit_on(ah->flags, AUDIO_PAUSED);
}

void music_set_paused(AudioHandler* ah, uint8_t p){
	if (p){
		Mix_PauseMusic();
		ah->flags = bit_on(ah->flags, AUDIO_PAUSED);
		return;
	}
	Mix_ResumeMusic();
	ah->flags = bit_off(ah->flags, AUDIO_PAUSED);
}

void audio_close(AudioHandler* ah){
	SoundMapIterator sit = SoundMapIteratorInit(&ah->sounds);
	while (SoundMapIteratorHasNext(&sit)){
		Mix_FreeChunk(SoundMapIteratorNext(&sit).val);
	}
	MusicMapIterator mit = MusicMapIteratorInit(&ah->music);
	while (MusicMapIteratorHasNext(&mit)){
		Mix_FreeMusic(MusicMapIteratorNext(&mit).val);
	}
	SoundMapFree(&ah->sounds);
	MusicMapFree(&ah->music);
	Mix_CloseAudio();
}

