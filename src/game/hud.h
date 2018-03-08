#pragma once
#include "../utils/font.h"
#include "../utils/sprite.h"
#include "../frameworks.h"

typedef struct {
  // TODO
} Hud;

Hud* createHud();
void destroyHud(Hud* hud);

void renderHud(SDL_Renderer* renderer, Hud* hud, SDL_Rect target, int frame);
