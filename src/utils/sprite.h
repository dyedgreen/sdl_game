#pragma once
#include "../frameworks.h"

// Animated sprite
typedef struct {
  int w;
  int h;
  int speed; // Frames per second to render (assuming 60 fps -> max 60)
  int frame; // This is the current frame in the animation
  int frame_count; // Is determined automatically (can be read, of animation)
  SDL_RendererFlip flip; // This can be set from the outside to flip the rendered image
  double rotation; // This can be set fron the outside to rotate the rendered image
  SDL_Texture* texture;
} Sprite;

Sprite* createSprite(SDL_Renderer* renderer, char* bmp, int frame_count, int speed);
void destroySprite(Sprite* sprite);

// This will advance the animation by one frame without rendering it
void advanceSprite(Sprite* sprite, int frame);
// This will render the current animation frame and advance the animation
void renderSprite(SDL_Renderer* renderer, Sprite* sprite, int frame, SDL_Rect* target_rect);
