#pragma once
#include "hand.h"
#include "../actions/play.h"
#include "../frameworks.h"

#define ENEMY_TURN_CARDS 3 // Max number of cards to be played

typedef struct {
  int player;
  int cards_played[ENEMY_TURN_CARDS];
  int current_card; // index in cards_played (goes 0->N)
  int current_anim;
  int current_exit_anim;
} EnemyTurn;

EnemyTurn* createEnemyTurn();
void destroyEnemyTurn(EnemyTurn* turn);

void resetEnemyTurn(EnemyTurn* turn, int player);
int advanceEnemyTurn(EnemyTurn* turn, Hand* hand, void* game_data_payload);
void renderEnemyTurn(SDL_Renderer* renderer, EnemyTurn* turn, Hand* hand, SDL_Rect target, int frame);
