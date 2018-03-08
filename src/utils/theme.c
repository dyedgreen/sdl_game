#include "theme.h"


Theme* createTheme(const char* audio_file) {
  Theme* theme = malloc(sizeof(Theme));
  if (theme == NULL) {
    return NULL;
  }
  // Load the audio file
  if (SDL_LoadWAV(audio_file, &theme->wav_spec, &theme->wav_buffer, &theme->wav_length) == NULL) {
    // Failed to load audio
    free(theme);
    return NULL;
  }
  // Open the audio device
  theme->wav_spec.callback = NULL;
  theme->device = SDL_OpenAudioDevice(NULL, 0, &theme->wav_spec, &theme->device_spec, 0);
  if (theme->device == 0) {
    // Failed to open device
    SDL_FreeWAV(theme->wav_buffer);
    free(theme);
    return NULL;
  }
  // Return the finished theme
  return theme;
}
void destroyTheme(Theme* theme) {
  if (theme == NULL) {
    return;
  }
  SDL_FreeWAV(theme->wav_buffer);
  SDL_CloseAudioDevice(theme->device);
  free(theme);
}

// This will make sure the theme is playing
void loopTheme(Theme* theme) {
  // Feed audio data to the device
  if (SDL_GetQueuedAudioSize(theme->device) == 0) {
    SDL_QueueAudio(theme->device, theme->wav_buffer, theme->wav_length);
  }
}
void pauseTheme(Theme* theme, int is_paused) {
  // Play / Pause audio
  SDL_PauseAudioDevice(theme->device, is_paused == 0 ? 0 : 1);
}
