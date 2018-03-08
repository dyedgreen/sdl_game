#include "enemy.h"

// Rendering constants and mechanic constants
#define ENEMY_TURN_DURATION 120 // Is effectively in frames/card play
#define ENEMY_TURN_CARDS 3 // How many cards to play on average

EnemyTurn* createEnemyTurn() {
  EnemyTurn* turn = malloc(sizeof(EnemyTurn));
  if (turn == NULL) {
    return NULL;
  }
  // TODO: Load some pictures of enemies, so that there is a small ui...
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
  // DEBUG: Currently the enemies try random moves... (not the greatest...)
  // Determine if a new play has to be found, of if the anim is advanced
  // if () {
  // 
  // } else if () {
  //
  // }
  // All is done
  return 0;
}

// Render the current turn state
void renderEnemyTurn(SDL_Renderer* renderer, EnemyTurn* turn, Hand* hand, SDL_Rect target, int frame) {

}
