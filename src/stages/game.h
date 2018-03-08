#pragma once
#import "stage.h"
#import "../utils/font.h"
#import "../utils/theme.h"
#import "../game/hand.h"
#import "../game/enemy.h"
#import "../game/map.h"
#import "../actions/cycle.h" // Progress map cycle (not actually used here -> remove)
#import "../actions/play.h"  // Play card function

// Game data (state, loaded textures etc...)
// NOTE: UPDATE IN PLAY.C!
typedef struct {
  Font* font;
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
