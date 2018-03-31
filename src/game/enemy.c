#include "enemy.h"

// Rendering constants and mechanic constants
#define ENEMY_TURN_DURATION 120 // Is effectively in frames/card play
#define ENEMY_TURN_EXIT_DURATION 20
#define ENEMY_RENDER_SCALE_FACTOR 600
#define ENEMY_RENDER_MAX_TILT 45
#define ENEMY_RENDER_MAX_OFFSET 100

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
  for (int i = 0; i < ENEMY_TURN_CARDS; i ++) {
    turn->cards_played[i] = -1;
  }
  turn->current_card = 0;
  turn->current_anim = 0;
  turn->current_exit_anim = 0;
}

// Go to next step in anim / turn (returns 0 when done...)
int advanceEnemyTurn(EnemyTurn* turn, Hand* hand, void* game_data_payload) {
  // DEBUG: Currently progresses through fake turn
  // Determine if a new play has to be found, of if the anim is advanced
  if (turn->cards_played[turn->current_card] == -1) {
    // Determine new card
    turn->cards_played[turn->current_card] = turn->current_card; // FIXME: Do some actual card generation and play here!
    return 1;
  } else if (turn->current_anim < ENEMY_TURN_DURATION) {
    // Advance the play
    turn->current_anim ++;
    return 1;
  }
  // Play next card
  if (turn->current_card < ENEMY_TURN_CARDS - 1) {
    turn->current_card ++;
    turn->current_anim = 0;
    return 1;
  }
  // Turn is done
  if (turn->current_exit_anim < ENEMY_TURN_EXIT_DURATION) {
    turn->current_exit_anim ++;
    return 1;
  }
  return 0;
}

// Curve for anim in
float renderEnemyTurnCurve(float x) {
  return 1.0 - powf(2.7, -20.0 * x);
}
// Render the current turn state
void renderEnemyTurn(SDL_Renderer* renderer, EnemyTurn* turn, Hand* hand, SDL_Rect target, int frame) {
  // Cards render target
  float render_scale = target.w / ENEMY_RENDER_SCALE_FACTOR;
  SDL_Rect card_render_target = {
    .w = hand->deck->cards[0]->sprite->w * render_scale,
    .h = hand->deck->cards[0]->sprite->h * render_scale,
    .x = 0,
    .y = 0,
  };
  // Each card played is rendered
  for (int i = 0; turn->cards_played[i] != -1 && i < ENEMY_TURN_CARDS; i ++) {
    // Determine progress in card array
    float shift = ((float)turn->current_card - (float)i + (float)turn->current_anim / (float)ENEMY_TURN_DURATION) / (float)(ENEMY_TURN_CARDS + 1);
    // Determine position
    card_render_target.x = target.x + target.w / 2 - card_render_target.w / 2;
    if (i == turn->current_card) {
      // Animate in
      card_render_target.x = (target.x + target.w / 2 - card_render_target.w / 2) * renderEnemyTurnCurve((float)turn->current_anim / (float)ENEMY_TURN_DURATION);
    } else {
      card_render_target.x = target.x + target.w / 2 - card_render_target.w / 2;
    }
    card_render_target.x += shift * (card_render_target.w + ENEMY_RENDER_MAX_OFFSET * render_scale);
    card_render_target.y = target.y + target.h / 2 - card_render_target.h / 2;
    card_render_target.y += shift * ENEMY_RENDER_MAX_OFFSET * render_scale;
    if (turn->current_exit_anim > 0) {
      card_render_target.y += (target.h + target.h) * turn->current_exit_anim * (ENEMY_TURN_CARDS - i) / ENEMY_TURN_EXIT_DURATION / 2;
    }
    // Determine rotation
    hand->deck->cards[turn->cards_played[i]]->sprite->rotation = shift * ENEMY_RENDER_MAX_TILT;
    // Render the card
    renderSprite(renderer, hand->deck->cards[turn->cards_played[i]]->sprite, frame, &card_render_target);
  }
}
