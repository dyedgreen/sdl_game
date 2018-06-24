#include "tile.h"

// Creates a tile (TODO: Game properties)
Tile* createTile(TileType type, int cave) {
  // Basic tile properties
  Tile* tile = malloc(sizeof(Tile));
  if (tile == NULL) {
    return NULL;
  }
  tile->type = type;
  tile->cave = cave ? 1 : 0;
  tile->player = -1; // Initially, this belongs to noone!
  tile->ignore_in_update = 0;
  // Tile game properties (start value will depend on tile type)
  return tile;
}

void destroyTile(Tile* tile) {
  // This is just here for uniformity, not strictly needed
  free(tile);
}

// Used for map generation
TileType determineTileType(float biome, float height, float nature) {
  // Override - water
  if (height < -0.5) {
    if (biome > 0.4) {
      // Dessert
      return Water;
    } else if (biome > 0.2) {
      // Swamp
      return Water;
    } else if (biome > -0.3) {
      // Grassland / Normal
      return Water;
    } else {
      // Snow
      if (height > -0.64) {
        return WaterIce;
      } else {
        return Water;
      }
    }
  }
  // Determine the biome (all inputs are from perlin -> [-1;1])
  if (biome > 0.4) {
    // Dessert
    if (height > 0.5) {
      return SandMountain;
    } else if (height > 0.3) {
      return SandStones;
    } else if (height > -0.5) {
      if (nature > 0.6) {
        return SandOasis;
      } else if (height > -0.1) {
        return SandDunes;
      } else {
        return Sand;
      }
    } else {
      return Sand;
    }
  } else if (biome > 0.2) {
    // Swamp
    if (height > 0.2) {
      if (nature > 0) {
        return SwampWoods;
      } else {
        return Swamp;
      }
    } else if (height > -0.5) {
      if (nature > 0.2) {
        return SwampWoods;
      } else if (nature > -0.2) {
        return Swamp;
      } else {
        return SwampGrass;
      }
    } else {
      return WaterSwamp;
    }
  } else if (biome > -0.3) {
    // Grassland / Normal
    if (height > 0.45) {
      return Mountain;
    } else if (height > 0.3) {
      if (nature > 0) {
        return GrassStonesWoods;
      } else {
        return GrassStones;
      }
    } else {
      if (nature > 0.34) {
        return rand() % 100 > 98 ? Ruins : GrassWoods;
      } else if (nature > -0.1) {
        return rand() % 100 > 99 ? Ruins : GrassLightWoods;
      } else {
        return Grass;
      }
    }
  } else {
    // Snow
    if (height > 0.45) {
      return Mountain;
    } else if (height > 0.3) {
      if (nature > 0) {
        return SnowStonesWoods;
      } else {
        return SnowStones;
      }
    } else {
      if (nature > 0.34) {
        return SnowWoods;
      } else if (nature > -0.1) {
        return SnowLightWoods;
      } else {
        return Snow;
      }
    }
  }
  // Fallback just in case...
  return WaterDeep;
}

void renderTile(Tile* tile, SDL_Texture* tileset, SDL_Renderer* renderer, SDL_Rect* target_rect) {
  // Determine the src on the tileset (assumed to be formatted as 8xN w=32, h=48)
  SDL_Rect src = {
    .w = 32,
    .h = 48,
    .x = 0,
    .y = 0,
  };
  // NOTE: tile type is pos on tileset!
  src.x = 32 * (tile->type % 8);
  src.y = 48 * (tile->type / 8);
  // Render the tile
  SDL_RenderCopy(renderer, tileset, &src, target_rect);
  // If the tile has a cave, render the correct cave
  if (tile->cave) {
    switch (tile->type) {
      // Grass cave
      case GrassLightWoods:
      case GrassStones:
      case GrassStonesWoods:
      case Mountain:
      case SwampWoods:
      case Swamp:
        src.x = 32 * (33 % 8);
        src.y = 48 * (33 / 8);
        SDL_RenderCopy(renderer, tileset, &src, target_rect);
        break;
      // Snow cave
      case SnowLightWoods:
      case SnowStones:
      case SnowStonesWoods:
        src.x = 32 * (34 % 8);
        src.y = 48 * (34 / 8);
        SDL_RenderCopy(renderer, tileset, &src, target_rect);
        break;
      // Sand cave
      case SandStones:
      case SandMountain:
        src.x = 32 * (35 % 8);
        src.y = 48 * (35 / 8);
        SDL_RenderCopy(renderer, tileset, &src, target_rect);
        break;
      default:
        // Do nothing, this tile can't have caves
        break;
    }
  }
}

void renderTileRoad(TileType road_type, SDL_Texture* tileset, SDL_Renderer* renderer, SDL_Rect* target_rect) {
  // Determine the src on the tileset (assumed to be formatted as 8xN w=32, h=48)
  SDL_Rect src = {
    .w = 32,
    .h = 48,
    .x = 0,
    .y = 0,
  };
  // NOTE: tile type is pos on tileset!
  src.x = 32 * (road_type % 8);
  src.y = 48 * (road_type / 8);
  // Render the tile
  SDL_RenderCopy(renderer, tileset, &src, target_rect);
}

// DURING GAME HELPER FUNCTIONS

int tileHasWood(Tile* tile) {
  // Returns number of wood
  switch (tile->type) {
    case GrassLightWoods:
    case GrassStonesWoods:
    case SwampWoods:
    case SnowStonesWoods:
    case SnowLightWoods:
      return 1;
    case GrassWoods:
    case SnowWoods:
      return 2;
    default:
      return 0;
  }
}
int tileDecreaseWood(Tile* tile) {
  // Returns success (true/false)
  switch (tile->type) {
    case GrassLightWoods:
      tile->type = Grass;
      break;
    case GrassWoods:
      tile->type = GrassLightWoods;
      break;
    case GrassStonesWoods:
      tile->type = GrassStones;
      break;
    case SwampWoods:
      tile->type = Swamp;
      break;
    case SnowLightWoods:
      tile->type = Snow;
      break;
    case SnowWoods:
      tile->type = SnowLightWoods;
      break;
    case SnowStonesWoods:
      tile->type = SnowStones;
      break;
    default:
      return 0;
      break;
  }
  return 1;
}
int tileHasWater(Tile* tile) {
  switch (tile->type) {
    case Water:
    case WaterDeep:
    case WaterSwamp:
    case WaterIce:
      return 1;
    default:
      return 0;
  }
}
int tileHasBuilding(Tile* tile) {
  switch(tile->type) {
    case GrassVillage:
    case GrassTown:
    case GrassCastle:
    case SnowVilage:
    case SnowCastle:
    case SandOasis:
    case SandCity:
    case SandCastle:
      return 1;
    default:
      return 0;
  }
}
