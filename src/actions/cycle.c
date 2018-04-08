#include "cycle.h"


int actionUpdateMapCycle(Map* map, int year) {
  // Update each tile
  for (int i = 0; i < map->width * map->height; i ++) {
    // Each tile type updates differentely
    switch (map->tiles[i]->type) {
      case GrassLightWoods:
      case SnowLightWoods:
        // We might grow a forest here (more dense wood is the next type!)
        if (rand() % 100 > 99) {
          map->tiles[i]->type += 1;
        }
        break;
      case GrassVillage:
      case GrassTown: {
        // Will try to plant up to 2 (3) crops on adjacent grass
        int max_crops = map->tiles[i]->type == GrassVillage ? 2 : 3;
        if (findMapTileNeighbourCrops(map, i) < max_crops && rand() % 100 > 50) {
          for (int offset = rand(), j = offset, neighbour = -1; j < offset + 6; j ++) {
            neighbour = findMapTileNeighbour(map, j % 6, i);
            if (map->tiles[neighbour]->type == Grass) {
              map->tiles[neighbour]->type = GrassCrops;
              map->tiles[neighbour]->player = map->tiles[i]->player;
              break;
            }
          }
        }
        break;
      }
      case SandVillage:
        // Catch for already updated tiles
        if (map->tiles[i]->ignore_in_update != 1) {
          // Wanders around the dessert in search for an oasis
          for (int offset = rand(), j = offset, neighbour = -1; j < offset + 6; j ++) {
            neighbour = findMapTileNeighbour(map, j % 6, i);
            if (map->tiles[neighbour]->type == Sand || map->tiles[neighbour]->type == SandDunes) {
              map->tiles[neighbour]->type = SandVillage;
              map->tiles[neighbour]->player = map->tiles[i]->player;
              map->tiles[i]->type = Sand;
              map->tiles[i]->player = -1;
              map->tiles[neighbour]->ignore_in_update = 1;
              break;
            } else if (map->tiles[neighbour]->type == SandOasis) {
              map->tiles[neighbour]->type = SandCity;
              map->tiles[neighbour]->player = map->tiles[i]->player;
              map->tiles[i]->type = Sand;
              map->tiles[i]->player = -1;
              map->tiles[neighbour]->ignore_in_update = 1;
              break;
            }
          }
        }
        break;
      default:
        // We do nothing for some tiles...
        break;
    }
  }
  // Reset the don't update flags
  for (int i = 0; i < map->width * map->height; i ++) {
    map->tiles[i]->ignore_in_update = 0;
  }
  // Might do something interesting here
  return year + 1;
}
