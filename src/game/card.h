#pragma once
#include "../frameworks.h"
#include "../utils/font.h"
#include "../utils/sprite.h"

typedef struct {
  int type; // The game is responsible for doing anything with the cards
  int cost; // How much cards need to be played in total (0 acts like 1)
  int sticky; // If == 1, the card WONT: Draw, Auto-discard (BUT: Are lost on shuffle etc.!)
  char title[12];
  char body[250];
  uint title_len;
  uint body_len;
  Sprite* sprite; // Created from card data
} Card;


Card* createCard(SDL_Renderer* renderer, char* picture, int anim_speed, char* title, char* body, int type, int cost, int sticky);
void destroyCard(Card* card);

// This will render a card destructing by play (returns true (1) when anim finishes)
int renderCardPlay(SDL_Renderer* renderer, Card* card, int frame, SDL_Point* target_point);
