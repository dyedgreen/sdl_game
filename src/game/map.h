#pragma once
#include "tile.h"
#include "../utils/perlin.h"
#include "../frameworks.h"

typedef enum {
  None=-1,
  Cross=0,
  BottomLeftTopRight=1,
  TopLeftBottomRight=2,
  TopLeftBottomLeft=3,
  TopRightBottomRight=4,
  BottomLeftBottomRight=5,
  TopLeftTopRight=6,
  BothLeftBottomRight=8,
  BottomLeftBothRight=9,
  BothLeftTopRight=10,
  TopLeftBothRight=11,
} RoadType;

typedef struct {
  // Map layout
  int width;
  int height;
  Tile** tiles; // Pointer to var length array
  RoadType* roads; // Has all the roads info
  // Render data
  SDL_Texture* tileset;
  SDL_Texture* tileset_border;
  SDL_Texture* tileset_highlight;
  SDL_Texture* tileset_roads;
  // Interaction data
  int tile_hovered;
  int tile_highlighted;
  Uint8 highlight_r;
  Uint8 highlight_g;
  Uint8 highlight_b;
  int scroll_x;
  int scroll_y;
  float scroll_normalize; // Make sure x/y scroll at equal speed
} Map;


// Create a map (TODO: will use internal map generator to build map to specifications)
Map* createMap(SDL_Renderer* renderer, int width, int height);
// Destroy a map
void destroyMap(Map* map);

// Render the map to the target
void renderMap(SDL_Renderer* renderer, Map* map, SDL_Rect target);
// Scroll map to new position
void scrollMap(Map* map, int offset_x, int offset_y);


// Game Helper Functions
int findMapTileNeighbour(Map* map, int neighbour, int tile);
int findMapTileNeighbourWood(Map* map, int tile);
int findMapTileNeighbourCrops(Map* map, int tile);
//int findMapTileVillage(Map* map, int player);

int findMapRoad(Map* map, int start, int end, int length); // Return if can be connected by road
int createMapRoad(Map* map, int start, int end, int length); // NOTE: This is (currently) permanent
