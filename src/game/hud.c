#include "hud.h"

// Render constants
// TODO: Design UI

Hud* createHud() {
  Hud* hud = malloc(sizeof(Hud));
  if (hud == NULL) {
    return NULL;
  }
  return hud;
}
void destroyHud(Hud* hud) {
  if (hud == NULL) {
    return;
  }
  free(hud);
}

void renderHud(SDL_Renderer* renderer, Hud* hud, SDL_Rect target, int frame) {

}
