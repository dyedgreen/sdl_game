#include "enemy.h"

// typedef struct {
//   int cards_played;
//   int current_card;
//   int current_anim;
//   int current_tile;
// } EnemyTurn;

EnemyTurn* createEnemyTurn() {
  EnemyTurn* turn = malloc(sizeof(EnemyTurn));
  if (turn == NULL) {
    return NULL;
  }
  resetEnemyTurn(turn, -1);
  return turn;
}
void destroyEnemyTurn(EnemyTurn* turn) {
  free(turn);
}

// Reset to begining of turn
void resetEnemyTurn(EnemyTurn* turn, int player) {
  turn->player = player;
  turn->cards_played = 0;
  turn->current_card = -1;
  turn->current_anim = -1;
  turn->current_tile = -1;
}

// Go to next step in anim / turn (returns 0 when done...)
int advanceEnemyTurn(EnemyTurn* turn, Hand* hand, void* game_data_payload) {
  return 0;
}

// Render the current turn state
void renderEnemyTurn(SDL_Renderer* renderer, EnemyTurn* turn, Hand* hand, SDL_Rect target, int frame) {

}
