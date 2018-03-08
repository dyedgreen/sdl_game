#pragma once
#include "deck.h"
#include "../frameworks.h"

// NOTE: Hand does not introduce any datatypes of it's own. It mearely manages the a deck and
//       renders the cards (also taking in mouse events etc.).
// NOTE: In hindsight: There is a data type introduced for following which cards are hovered or
//       clicked.
// NOTE: Hand is also used by the enemy (however, the enemy only uses the deck, while during
//       the enemies turn, the players hand is completely empty).

typedef struct {
  int card_hovered; // current card hovered, -1 is none is hovered
  int card_selected; // card being played, -1 is none is selected
  int card_hover_progress[DECK_LENGTH]; // -100 -> 0 -> 100, how much the card should stick out
  int card_remove_progress[DECK_LENGTH]; // 0 -> 100, to animate the cards being removed
  int card_follow_distance[DECK_LENGTH]; // 0 -> 100, how far from the cursor the card should be rendered
  Deck* deck;
  int (*play)(int, void*); // Passes the card type to the game; if true (!= 0) is returned, the card is removed from the hand
} Hand;

Hand* createHand(SDL_Renderer* renderer, int (*play)(int, void*));
void destroyHand(Hand* hand);

void renderHand(SDL_Renderer* renderer, Hand* hand, SDL_Rect target, int frame);
void updateHand(Hand* hand, SDL_Event* e, void* payload);
void fillHand(Hand* hand, int full_count); // Fill hand up to n cards (ignores sticky)
void drawCards(Hand* hand, int count); // Draw n random cards (ignores sticky)
void discardCards(Hand* hand, int count); // Discards random cards (ignores sticky)
void drawCard(Hand* hand, int id); // Draws a specific card to the hand
void reshuffleHand(Hand* hand, int full_count);
