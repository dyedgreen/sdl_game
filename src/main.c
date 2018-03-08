#include "main.h"


// This is the state for the scenes
Stage* stages[1];
int current_stage = 0;

void replaceCurrentStage(int new_stage) {
  if (new_stage < lengthof(stages) && current_stage >= 0) {
    current_stage = new_stage;
  }
}

int main(int argc, char* argv[]) {

  // Declare pointers for window and renderer
  SDL_Window* window;
  SDL_Renderer* renderer;

  // Init SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("There was a problem initialising SDL.");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "There was a problem initialising SDL.", NULL);
    return 1;
  }

  // Create an application window with the following settings:
  window = SDL_CreateWindow(
      "Game (Change Later)",                      // window title
      SDL_WINDOWPOS_UNDEFINED,                    // initial x position
      SDL_WINDOWPOS_UNDEFINED,                    // initial y position
      640,                                        // width, in pixels
      480,                                        // height, in pixels
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE    // flags - see below
  );

  // Check that the window was successfully created
  if (window == NULL) {
    printf("Could not create window: %s\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not create window.", NULL);
    return 1;
  }

  // Init the 2d rendering
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

  // Check that the renderer was created successfully
  if (renderer == NULL) {
    printf("Could not create renderer.");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not create renderer.", window);
    SDL_DestroyWindow(window);
    return 1;
  }

  // Seed the random generator
  srand(time(NULL));

  // Create all stages
  stages[0] = createGameStage(&replaceCurrentStage, window, renderer);

  // This are the variables needed for the update loop
  SDL_Event e;
  int running = 1;
  int frame = 0;

  // Make sure all stages could be created
  for (int i = 0; i < lengthof(stages); i ++) {
    if (stages[i] == NULL) {
      running = 0; // Prevent game from starting
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load game.", window);
    }
  }

  // This is the update loop that runs every frame
  while (running) {
    // Get all events
    while(SDL_PollEvent(&e)) {
      // Quit the app if requested
      if (e.type == SDL_QUIT) {
        // Quit here
        running = 0;
      }
      // Pass the event on to the current scene
      stages[current_stage]->event(stages[current_stage]->scene, &e);
    }
    // Call the current scenes update
    stages[current_stage]->update(stages[current_stage]->scene, frame);
    // Wait some time (->60 FPS)
    SDL_Delay(16);
    frame ++;
    frame = frame % 60;
  }

  // Destroy all stages
  destroyGameStage(stages[0]);

  // Close and destroy the renderer
  SDL_DestroyRenderer(renderer);
  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}
