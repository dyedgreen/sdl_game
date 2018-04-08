#include "deck.h"

// Builds the deck
Deck* createDeck(SDL_Renderer* renderer) {
  // Allocate the deck
  Deck* deck = malloc(sizeof(Deck));
  if (deck == NULL) {
    return NULL;
  }
  // Allocate the cards (hard-coded here, maybe change later?)
  // Cheat-Cheat:
  // renderer, picture, anim_speed, title (12), body (250), type, cost, sticky
  // DEBUG
  deck->cards[0] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "Debug",
    "This is a test card.",
    0, 1, 0);
  // SETTLERS CARD
  deck->cards[1] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "Settlers",
    "A few people looking to build a new life in a friendly grassland. To build a village, they require trees to be around.",
    2, 2, 0);
  // TOWN CARD (upgrade settlers)
  deck->cards[2] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "Town",
    "This upgrades a village (TODO)",
    4, 3, 0);
  // CASTLE CARD
  deck->cards[3] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "Castle",
    "This upgrades a town (TODO)",
    5, 4, 0);
  // NOMADS CARD
  deck->cards[4] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "Nomads",
    "These people wander the dessert with their tents and livestock in a search for an oasis. When they find it, they'll build a city around it.",
    3, 3, 0);
  // END TURN CARD
  deck->cards[5] = createCard(renderer,
    "assets/cards/test_card_anim.bmp", 5,
    "End Turn",
    "This card ends the turn.",
    1, 0, 1);
  // Fill card used
  for (int i = 0; i < DECK_LENGTH; i ++) {
    deck->card_used[i] = 0;
    // Test if card was created
    if (deck->cards[i] == NULL) {
      destroyDeck(deck);
      return NULL;
    }
  }
  // Return the deck
  return deck;
}

// Empties the deck
void destroyDeck(Deck* deck) {
  if (deck == NULL) {
    return;
  }
  // Destroy all cards
  for (int i = 0; i < lengthof(deck->cards); i ++) {
    destroyCard(deck->cards[i]);
  }
  // Free the deck memory
  free(deck);
}
