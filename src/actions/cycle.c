#include "cycle.h"


int actionUpdateMapCycle(Map* map, int year) {
  // Update each tile
  for (int i = 0; i < map->width * map->height; i ++) {
    // Each tile type updates differentely
    switch (map->tiles[i]->type) {
      case GrassLightWoods:
      case SnowLightWoods:
        // We might grow a forrest here (more dense wood is the next type!)
        if (rand() % 100 > 99) {
          map->tiles[i]->type += 1;
        }
        break;
      case GrassVillage:
        // Will try to plant up to 2 crops on adjacent grass
        if (findMapTileNeighbourCrops(map, i) < 2 && rand() % 100 > 50) {
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
      default:
        // We do nothing for some tiles...
        break;
    }
  }
  // Might do something interesting here
  return year + 1;
}
