#pragma once
#include "../frameworks.h"

typedef enum {
  Grass=0,
  GrassLightWoods=1,
  GrassWoods=2,
  GrassStones=3,
  GrassStonesWoods=4,
  Mountain=5,
  Water=6,
  WaterDeep=7,
  WaterSwamp=15,
  WaterIce=21,
  GrassVillage=8,
  GrassTown=9,
  GrassCastle=10,
  GrassCrops=11,
  SwampWoods=12,
  Swamp=13,
  SwampGrass=14,
  Snow=16,
  SnowLightWoods=17,
  SnowWoods=18,
  SnowStones=19,
  SnowStonesWoods=20,
  SnowVilage=22,
  SnowCastle=23,
  Sand=24,
  SandStones=25,
  SandDunes=26,
  SandMountain=27,
  SandOasis=28,
  SandVillage=29,
  SandCity=30,
  SandCastle=31,
  Jungle=32,
} TileType;

typedef struct {
  // Tile type and render stuff
  TileType type;
  int cave;
  // Tile game properties TODO
  int player; // Player who owns the tile (-1 -> no owner)
  int ignore_in_update; // 0 -> NO; 1 -> YES
  //int resource_wood;
  //int resource_food;
  //int population;
} Tile;


// Create a tile
Tile* createTile(TileType type, int cave);
// Destroy a tile
void destroyTile(Tile* tile);
// Determine a tile type based on height-maps
TileType determineTileType(float biome, float height, float nature);
// Render a tile (needs a valid tile structure + target + renderer)
void renderTile(Tile* tile, SDL_Texture* tileset, SDL_Renderer* renderer, SDL_Rect* target_rect);
void renderTileRoad(TileType road_type, SDL_Texture* tileset, SDL_Renderer* renderer, SDL_Rect* target_rect);


// Tile helper function (for during the game)
int tileHasWood(Tile* tile);
int tileDecreaseWood(Tile* tile);
int tileHasWater(Tile* tile);
int tileHasBuilding(Tile* tile);
