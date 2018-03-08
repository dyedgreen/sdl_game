#pragma once
#include "../frameworks.h"

// A theme song that should be looped
typedef struct {
  // Data for the audio file
  SDL_AudioSpec wav_spec;
  Uint32 wav_length;
  Uint8* wav_buffer;
  // Data for the audio device
  SDL_AudioSpec device_spec;
  SDL_AudioDeviceID device;
} Theme;

Theme* createTheme(const char* audio_file);
void destroyTheme(Theme* theme);

// This will make sure the theme is playing (TODO: add play/pause later?)
void loopTheme(Theme* theme);
// This will play/pause the theme
void pauseTheme(Theme* theme, int is_paused);
