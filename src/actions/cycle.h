// NOTE: The cycle action is executed at the end of every turn
//       and updates the map according to the maps current
//       state.
//       Also, in the current system design, this is invoked by
//       playing the 'end of turn' card.
#pragma once
#include "../frameworks.h"
#include "../game/tile.h"
#include "../game/map.h"

// Updates the map cycle and returns the new year
int actionUpdateMapCycle(Map* map, int year);
