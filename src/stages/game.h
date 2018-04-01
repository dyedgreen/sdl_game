#pragma once
#include "stage.h"
#include "../utils/font.h"
#include "../utils/ui.h"
#include "../utils/theme.h"
#include "../game/hand.h"
#include "../game/enemy.h"
#include "../game/map.h"
#include "../actions/cycle.h" // Progress map cycle (not actually used here -> remove)
#include "../actions/play.h"  // Play card function

// Game data (state, loaded textures etc...)
// NOTE: UPDATE IN PLAY.C!
typedef struct {
  Font* font;
  Ui* ui;
  Theme* theme;
  Hand* hand;
  EnemyTurn* enemy_turn;
  Map* map;
  int year; // NOTE: Keeps track of whose turn it is... (year % no. of players)
} GameData;

// Create the game scene
Stage* createGameStage(void (*replace)(int), SDL_Window* window, SDL_Renderer* renderer);
// Destroy a game Scene
void destroyGameStage(Stage* stage);
