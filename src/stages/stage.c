#import "stage.h"

// Create a new scene
Stage* createStage(void (*update)(Scene*, int), void (*event)(Scene* , SDL_Event*), void (*replace)(int), SDL_Window* window, SDL_Renderer* renderer) {
  // Allocate the stage
  Stage* stage = malloc(sizeof(Stage));
  if (stage == NULL) {
    return NULL;
  }
  // Assign the update and event callbacks
  stage->update = update;
  stage->event = event;
  // Allocate the scene for the stage
  stage->scene = malloc(sizeof(Scene));
  if (stage->scene == NULL) {
    free(stage);
    return NULL;
  }
  stage->scene->replace = replace;
  stage->scene->window = window;
  stage->scene->renderer = renderer;
  // Return the finished stage
  return stage;
}

// Destroy an existing scene
void destroyStage(Stage* stage) {
  if (stage == NULL) {
    return;
  }
  // Free the scene
  free(stage->scene);
  // Free the stage
  free(stage);
}
