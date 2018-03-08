#pragma once
#include "../frameworks.h"

// Scene Struct
typedef struct {
  void (*replace)(int); // This may be called to update the stage responsible for managing the window
  void* data; // This can be used by the specific scene creation func to allocate extra state
  SDL_Window* window;
  SDL_Renderer* renderer;
} Scene;
// Stage (scene with callbacks) struct
typedef struct {
  void (*update)(Scene*, int); // Called every frame (int is the frame, 1->60)
  void (*event)(Scene* , SDL_Event*); // Called before each frame
  Scene* scene;
} Stage;

Stage* createStage(void (*update)(Scene*, int), void (*event)(Scene* , SDL_Event*), void (*replace)(int), SDL_Window* window, SDL_Renderer* renderer);
void destroyStage(Stage* stage);
