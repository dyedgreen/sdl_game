#pragma once
#include "hand.h"
#include "../actions/play.h"
#include "../frameworks.h"

typedef struct {
  int player;
  int cards_played;
  int current_card;
  int current_anim;
  int current_tile;
} EnemyTurn;

EnemyTurn* createEnemyTurn();
void destroyEnemyTurn(EnemyTurn* turn);

void resetEnemyTurn(EnemyTurn* turn, int player);
int advanceEnemyTurn(EnemyTurn* turn, Hand* hand, void* game_data_payload);
void renderEnemyTurn(SDL_Renderer* renderer, EnemyTurn* turn, Hand* hand, SDL_Rect target, int frame);
