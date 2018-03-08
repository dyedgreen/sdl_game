#pragma once
#include "card.h"
#include "../frameworks.h"

#define DECK_LENGTH 6

typedef struct {
  Card* cards[DECK_LENGTH];
  int card_used[DECK_LENGTH]; // i.e. in hand
} Deck;


Deck* createDeck(SDL_Renderer* renderer); // Currently hard-coded
void destroyDeck(Deck* deck);
