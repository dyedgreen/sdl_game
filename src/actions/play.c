#include "play.h"

// Constants
#define PLAY_ROAD_LENGTH 10


// Declared separately since they cause header collisions otherwise
// NOTE: This is suboptimal, but it is important to make sure the CardType values correspond
//       to the values in deck.c 'createDeck' and that the game-data struct is always synced
//       with the master version in game.c
typedef struct {
  Font* font;
  Ui* ui;
  Theme* theme;
  Hand* hand;
  EnemyTurn* enemy_turn;
  Map* map;
  int year;
} GameData;


int actionCanPlayCard(int card_type, int player, void* game_data_payload) {
  GameData* game_data = (GameData*)game_data_payload;
  // Guard against invalid tile hovered
  if (
    game_data->map->tile_hovered == -1 ||
    game_data->map->tile_hovered < 0 ||
    game_data->map->tile_hovered >= game_data->map->width * game_data->map->height
  ) {
    return 0;
  }
  // Action is card dependent
  switch ((CardType)card_type) {
    // DEBUG
    case Debug:
      return 1;
      break;
    // END TURN CARD
    case EndTurn:
      return 1;
    // SETTLERS CARD
    case Settlers:
      // Has to be a grassland and there needs to be wood around
      if (
        game_data->map->tiles[game_data->map->tile_hovered]->type == Grass &&
        findMapTileNeighbourWood(game_data->map, game_data->map->tile_hovered) >= 2
      ) {
        // Find player villages and test if they can be reached by roads
        int buildings = 0;
        for (int i = 0; i < game_data->map->width * game_data->map->height; i ++) {
          if (
            game_data->map->tiles[i]->player == player &&
            tileHasBuilding(game_data->map->tiles[i])
          ) {
            buildings ++;
            if (findMapRoad(game_data->map, game_data->map->tile_hovered, i, PLAY_ROAD_LENGTH)) {
              return 1;
            }
          }
        }
        return buildings == 0 ? 1 : 0;
      } else {
        return 0;
      }
      break;
    // TOWN CARD
    case Town:
      // Has to be a Village with one crops + one wood
      if (
        game_data->map->tiles[game_data->map->tile_hovered]->type == GrassVillage &&
        findMapTileNeighbourCrops(game_data->map, game_data->map->tile_hovered) >= 1 &&
        findMapTileNeighbourWood(game_data->map, game_data->map->tile_hovered) >= 1
      ) {
        return 1;
      } else {
        return 0;
      }
      break;
    // CASTLE CARD
    case Castle:
      // Has to be a Town with 3 crops + one wood
      if (
        game_data->map->tiles[game_data->map->tile_hovered]->type == GrassTown &&
        findMapTileNeighbourCrops(game_data->map, game_data->map->tile_hovered) >= 3 &&
        findMapTileNeighbourWood(game_data->map, game_data->map->tile_hovered) >= 1
      ) {
        return 1;
      } else {
        return 0;
      }
      break;
    // NOMADS CARD
    case Nomads:
      // Has to be dessert
      if (
        game_data->map->tiles[game_data->map->tile_hovered]->type == Sand ||
        game_data->map->tiles[game_data->map->tile_hovered]->type == SandDunes
      ) {
        return 1;
      }
      return 0;
      break;
    default:
      // Card not known
      return 0;
      break;
  }
}

int actionPlayCard(int card_type, int player, void* game_data_payload) {
  GameData* game_data = (GameData*)game_data_payload;
  if (!actionCanPlayCard(card_type, player, game_data_payload)) {
    // This guards against non-valid plays!
    return 0;
  }
  // Action is card dependent
  switch ((CardType)card_type) {
    // DEBUG
    case Debug:
      // Does nothing...
      break;
    // END TURN CARD
    case EndTurn:
      // Discard all cards, update the map cycle (grows crops etc.)
      reshuffleHand(game_data->hand, 0);
      game_data->year = actionUpdateMapCycle(game_data->map, game_data->year);
      break;
    // SETTLERS CARD
    case Settlers: {
      // Try to build a road here
      int has_road = 0;
      int buildings = 0;
      for (int i = 0; i < game_data->map->width * game_data->map->height; i ++) {
        if (
          game_data->map->tiles[i]->player == player &&
          tileHasBuilding(game_data->map->tiles[i])
        ) {
          buildings ++;
          if (
            findMapRoad(game_data->map, game_data->map->tile_hovered, i, PLAY_ROAD_LENGTH) &&
            createMapRoad(game_data->map, game_data->map->tile_hovered, i, PLAY_ROAD_LENGTH)
          ) {
            has_road = 1;
            break;
          }
        }
      }
      // Catch edge cases...
      if (!has_road && buildings > 0) {
        return 0;
      }
      // Place a city here, if there is a forest around (decreases the wood, guaranteed 2 wood by can play)
      int offset = rand();
      int i = offset;
      int removed = 0;
      int neighbour = -1;while (i < offset + 6 && removed < 2) {
        neighbour = findMapTileNeighbour(game_data->map, i % 6, game_data->map->tile_hovered);
        if (neighbour != -1) {
          removed += tileDecreaseWood(game_data->map->tiles[neighbour]);
        }
        i ++;
      }
      game_data->map->tiles[game_data->map->tile_hovered]->type = GrassVillage;
      game_data->map->tiles[game_data->map->tile_hovered]->player = player;
      break;
    // TOWN CARD
    case Town: 
      // Remove one wood
      for (int i = 0, removed = 0, neighbour; i < 6 && removed < 1; i ++) {
        neighbour = findMapTileNeighbour(game_data->map, i, game_data->map->tile_hovered);
        if (neighbour != -1) {
          removed += tileDecreaseWood(game_data->map->tiles[neighbour]);
        }
      }
      game_data->map->tiles[game_data->map->tile_hovered]->type = GrassTown;
      return 1;
    // CASTLE CARD
    case Castle:
      // Remove one wood
      for (int i = 0, removed = 0, neighbour; i < 6 && removed < 1; i ++) {
        neighbour = findMapTileNeighbour(game_data->map, i, game_data->map->tile_hovered);
        if (neighbour != -1) {
          removed += tileDecreaseWood(game_data->map->tiles[neighbour]);
        }
      }
      game_data->map->tiles[game_data->map->tile_hovered]->type = GrassCastle;
      return 1;
      break;
    // NORMADS CARD
    case Nomads:
      // Place the nomads and claim tile (valid is guaranteed above)
      game_data->map->tiles[game_data->map->tile_hovered]->type = SandVillage;
      game_data->map->tiles[game_data->map->tile_hovered]->player = player;
      return 1;
    }
    default:
      // Card not known
      break;
  }
  // first test
  return 1;
}
