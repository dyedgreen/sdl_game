// NOTE: The play action is invoked whenever a card is played.
//       (Essentially, the play action simply moves the play fn
//       from the game.c to a separate file.)
#pragma once
#include "../frameworks.h"
#include "cycle.h"
#include "../game/hand.h"
#include "../game/enemy.h"
#include "../game/map.h"
#include "../utils/font.h"
#include "../utils/ui.h"
#include "../utils/theme.h"


// Plays a card and returns if the play is valid
int actionCanPlayCard(int card_type, int player, void* game_data_payload);
int actionPlayCard(int card_type, int player, void* game_data_payload);
