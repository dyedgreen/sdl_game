#include "sprite.h"

// Create a new sprite
Sprite* createSprite(SDL_Renderer* renderer, char* bmp, int frame_count, int speed) {
  // Allocate the sprite
  Sprite* sprite = malloc(sizeof(Sprite));
  if (sprite == NULL) {
    return NULL;
  }
  // Set the initial values
  sprite->frame_count = frame_count;
  sprite->speed = speed;
  sprite->frame = 0;
  sprite->flip = SDL_FLIP_NONE;
  sprite->rotation = 0.0;
  // Load the texture
  SDL_Surface* bitmap = SDL_LoadBMP(bmp);
  sprite->texture = SDL_CreateTextureFromSurface(renderer, bitmap);
  if (sprite->texture == NULL) {
    free(sprite);
    SDL_FreeSurface(bitmap);
    return NULL;
  }
  SDL_SetTextureBlendMode(sprite->texture, SDL_BLENDMODE_BLEND);
  // Calculate the frame width and height
  sprite->w = bitmap->w / frame_count;
  sprite->h = bitmap->h;
  // Free the surface and return
  SDL_FreeSurface(bitmap);
  return sprite;
}

void destroySprite(Sprite* sprite) {
  if (sprite == NULL) {
    return;
  }
  // Destroy the texture
  SDL_DestroyTexture(sprite->texture);
  // Free the sprite data
  free(sprite);
}

// This will advance the animation by one frame without rendering it
void advanceSprite(Sprite* sprite, int frame) {
  // Check for 0 speed
  if (sprite->speed == 0) {
    return;
  }
  // Advance the correct times per frame
  if (frame % (60 / sprite->speed) == 0) {
    sprite->frame = (sprite->frame + 1) % sprite->frame_count;
  }
}
// This will render the current animatino frame and advance the animation
void renderSprite(SDL_Renderer* renderer, Sprite* sprite, int frame, SDL_Rect* target_rect) {
  // Render the correct frame to the target
  SDL_Rect src_rect;
  src_rect.w = sprite->w;
  src_rect.h = sprite->h;
  src_rect.y = 0;
  src_rect.x = sprite->w * sprite->frame;
  // Determine if there is any rotation of flip to be applied
  if (sprite->rotation != 0 || sprite->flip != SDL_FLIP_NONE) {
    SDL_RenderCopyEx(renderer, sprite->texture, &src_rect, target_rect, sprite->rotation, NULL, sprite->flip);
  } else {
    SDL_RenderCopy(renderer, sprite->texture, &src_rect, target_rect);
  }
  // Advance the frame count
  advanceSprite(sprite, frame);
}
