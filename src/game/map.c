#import "map.h"

#define MAP_HIGHLIGHT_ALPHA 100
#define MAP_SCROLL_MAX 100
#define MAP_SCROLL_STEP 1

// Main terrain height mask (forms a circular island -> streched to ellipse by map render positions)
float heightMask(float x, float y, float w, float h, float p) {
  // Generate an island
  float distance = sqrt((w/2 - x)*(w/2 - x)*1.2 + (h/2 - y)*(h/2 - y));
  distance = distance * 1.7 / sqrt(w*w/4 + h*h/4);
  float res = 1;
  for (int i = 0; i < 5; i ++) {
    res *= distance;
  }
  return (p + 1.0) * (1.0 - res) - 1.0;
}

// Generate a map (returns an array of types)
void generateMap(TileType* map_prototype[], int width, int height) {
  // Create each tile
  int seed = rand(); // Nice seeds: 7933, 6689
  for (int y = 0; y < height; y ++) {
    for (int x = 0; x < width; x ++) {
      // Determine a tile based on different perlin-noise maps
      (*map_prototype)[y * width + x] = determineTileType(
        perlin(x*10, y*10, 0.013, seed), // BIOME
        heightMask((float)x, (float)y, (float)width, (float)height, perlin(x*10, y*10, 0.025, seed * seed - seed)), // HEIGHT
        perlin(x*10, y*10, 0.04, seed / 3 + seed) // NATURE
      );
    }
  }
}

// Create a map
Map* createMap(SDL_Renderer* renderer, int width, int height) {
  // Generate a prototype map, that contains all the tile types of the final map
  int map_size = width * height;
  TileType* map_prototype = malloc(sizeof(TileType) * map_size);
  if (map_prototype == NULL) {
    return NULL;
  }
  generateMap(&map_prototype, width, height);
  // Allocate the map and tiles and roads arrays
  Map* map = malloc(sizeof(Map));
  Tile** tiles = malloc(sizeof(Tile*) * map_size);
  RoadType* roads = malloc(sizeof(RoadType) * map_size);
  if (map == NULL || tiles == NULL || roads == NULL) {
    free(map_prototype);
    free(map);
    free(tiles);
    free(roads);
    return NULL;
  }
  // Create all tiles and determine whether they have a canyon
  for (int i = 0; i < map_size; i ++) {
    tiles[i] = createTile(map_prototype[i], rand() % 100 > 97);
    roads[i] = None;
    // Test if tile was created (and free everything if it failed!)
    if (tiles[i] == NULL) {
      for (int j = 0; j < i+1; j ++) {
        destroyTile(tiles[i]);
      }
      free(map_prototype);
      free(map);
      free(tiles);
      free(roads);
      return NULL;
    }
  }
  // Remove the map prototype
  free(map_prototype);
  map->tiles = tiles;
  map->roads = roads;
  // Set map data values
  map->width = width;
  map->height = height;
  map->tile_hovered = -1;
  map->tile_highlighted = -1;
  map->highlight_r = 255;
  map->highlight_g = 255;
  map->highlight_b = 255;
  map->scroll_x = 0;
  map->scroll_y = 0;
  map->scroll_normalize = 1.0;
  // Load the tileset textures
  SDL_Surface* bitmap_tileset = SDL_LoadBMP("assets/map/tiles.bmp");
  SDL_Surface* bitmap_tileset_border = SDL_LoadBMP("assets/map/tiles_border.bmp");
  SDL_Surface* bitmap_tileset_highlight = SDL_LoadBMP("assets/map/tiles_highlight.bmp");
  SDL_Surface* bitmap_tileset_roads = SDL_LoadBMP("assets/map/tiles_roads.bmp");
  map->tileset = SDL_CreateTextureFromSurface(renderer, bitmap_tileset);
  map->tileset_border = SDL_CreateTextureFromSurface(renderer, bitmap_tileset_border);
  map->tileset_highlight = SDL_CreateTextureFromSurface(renderer, bitmap_tileset_highlight);
  map->tileset_roads = SDL_CreateTextureFromSurface(renderer, bitmap_tileset_roads);
  // Free the bitmaps
  SDL_FreeSurface(bitmap_tileset);
  SDL_FreeSurface(bitmap_tileset_border);
  SDL_FreeSurface(bitmap_tileset_highlight);
  SDL_FreeSurface(bitmap_tileset_roads);
  // Test if all textures could be created
  if (map->tileset == NULL || map->tileset_border == NULL || map->tileset_highlight == NULL || map->tileset_roads == NULL) {
    destroyMap(map);
    return NULL;
  }
  SDL_SetTextureBlendMode(map->tileset, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(map->tileset_border, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(map->tileset_highlight, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(map->tileset_roads, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(map->tileset_highlight, MAP_HIGHLIGHT_ALPHA);
  // Return the map
  return map;
}
// Destroy a map
void destroyMap(Map* map) {
  if (map == NULL) {
    return;
  }
  // Free all tiles
  for (int i = 0; i < map->width*map->height; i ++) {
    destroyTile(map->tiles[i]);
  }
  // Free the tile list and textures
  free(map->tiles);
  free(map->roads);
  SDL_DestroyTexture(map->tileset);
  SDL_DestroyTexture(map->tileset_border);
  SDL_DestroyTexture(map->tileset_highlight);
  SDL_DestroyTexture(map->tileset_roads);
  // Free map struct
  free(map);
}

// Render the map to the target rect (where target is typically whole screen)
void renderMap(SDL_Renderer* renderer, Map* map, SDL_Rect target) {
  // Determine the tile width and height
  int width_w = 4 * target.w / (6*map->width - 1);
  int height_h = 2 * target.h / (map->height - 1);
  // Make sure the width and height are multiples of the true size (surpresses rounding artifacts)
  width_w = (width_w / 32 + 1) * 32;
  height_h = (height_h / 28 + 1) * 28;
  int height_w = width_w * 28 / 32;
  int width_h = height_h * 32 / 28;
  // We use the larger result as our tile hexagon width / height
  int tile_w = width_w > width_h ? width_w : width_h;
  int tile_h = height_w > height_h ? height_w : height_h;
  int scroll_max_x = tile_w * map->width * 3 / 2 - tile_w / 4 - target.w;
  int scroll_max_y = tile_h * (map->height - 1) / 2 - target.h;
  map->scroll_normalize = (float)scroll_max_x / (float)scroll_max_y;
  SDL_Rect tile_target = {
    .w = tile_w,
    .h = tile_h * 48 / 28,
    .x = 0,
    .y = 0,
  };

  // Get the position of the mouse
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);

  // Render all VISIBLE (yay :) tiles from with to height
  // NOTE: y/x are map-elements from top to bottom / left to right
  int max_y = (2 * (target.h + scroll_max_y * map->scroll_y / MAP_SCROLL_MAX) + tile_h) / tile_h + 1;
  int max_x = (4 * (target.w + scroll_max_x * map->scroll_x / MAP_SCROLL_MAX) + tile_w) / 6 / tile_w + 1;
  max_y = map->height < max_y ? map->height : max_y;
  max_x = map->width < max_x ? map->width : max_x;
  int min_y = (2 * (scroll_max_y * map->scroll_y / MAP_SCROLL_MAX) + tile_h) / tile_h - 1;
  int min_x = (4 * (scroll_max_x * map->scroll_x / MAP_SCROLL_MAX) + tile_w) / 6 / tile_w - 1;
  min_y = 0 > min_y ? 0 : min_y;
  min_x = 0 > min_x ? 0 : min_x;
  for (int y = min_y; y < max_y; y ++) {
    for (int x = min_x; x < max_x; x ++) {
      // Move x and y to target + apply scroll
      tile_target.x = target.x - scroll_max_x * map->scroll_x / MAP_SCROLL_MAX;
      tile_target.y = target.y - scroll_max_y * map->scroll_y / MAP_SCROLL_MAX - tile_h / 28;
      // Different rows are treated as different tile-shapes conceptually
      if (y % 2 == 0) {
        // Determine position x and y of tile
        tile_target.x += x * tile_w + x * tile_w / 2 - tile_w / 4;
        tile_target.y += y * tile_h / 2 - tile_h / 2;
        // Determine if the tile is hovered
        if (
          tile_target.x < mouse_x && tile_target.y < mouse_y &&
          tile_target.x + tile_w > mouse_x && tile_target.y + tile_h > mouse_y
        ) {
          map->tile_hovered = y * map->width + x;
        }
        // Shift tile x/y to fit actual tile bitmap
        tile_target.y -= tile_h * 18 / 28;
      } else {
        // Determine position x and y of tile
        tile_target.x += x * tile_w + x * tile_w / 2 + tile_w / 2;
        tile_target.y += y * tile_h / 2 - tile_h / 2;
        // Determine if the tile is hovered
        if (
          tile_target.x + tile_w / 4 < mouse_x && tile_target.y < mouse_y &&
          tile_target.x + tile_w / 2 > mouse_x && tile_target.y + tile_h > mouse_y
        ) {
          map->tile_hovered = y * map->width + x;
        }
        // Shift tile x/y to fit actual tile bitmap
        tile_target.y -= tile_h * 18 / 28;
      }
      // Render the tile (water is plain, all other has borders)
      if (tileHasWater(map->tiles[y * map->width + x])) {
        renderTile(map->tiles[y * map->width + x], map->tileset, renderer, &tile_target);
      } else {
        renderTile(map->tiles[y * map->width + x], map->tileset_border, renderer, &tile_target);
      }
      // Render the road is present
      if (map->roads[y * map->width + x] != None && !tileHasBuilding(map->tiles[y * map->width + x])) {
        renderTileRoad((TileType)map->roads[y * map->width + x], map->tileset_roads, renderer, &tile_target);
      }
      // If this tile is highlighted, render the highlight
      if (map->tile_highlighted == y * map->width + x) {
        SDL_SetTextureColorMod(map->tileset_highlight, map->highlight_r, map->highlight_g, map->highlight_b);
        renderTile(map->tiles[y * map->width + x], map->tileset_highlight, renderer, &tile_target);
      }
    }
  }
}

// Scroll map to new x/y offset
void scrollMap(Map* map, int offset_x, int offset_y) {
  // Move the offset
  map->scroll_x += offset_x * MAP_SCROLL_STEP;
  map->scroll_y += (int)((float)(offset_y * MAP_SCROLL_STEP) * map->scroll_normalize);
  // Confine to bounds
  map->scroll_x = map->scroll_x < 0 ? 0 : map->scroll_x;
  map->scroll_x = map->scroll_x > MAP_SCROLL_MAX ? MAP_SCROLL_MAX : map->scroll_x;
  map->scroll_y = map->scroll_y < 0 ? 0 : map->scroll_y;
  map->scroll_y = map->scroll_y > MAP_SCROLL_MAX ? MAP_SCROLL_MAX : map->scroll_y;
}


// HELPER FUNCTIONS

// Find the neigbours of a tile (-1 = no neighbours)
int findMapTileNeighbour(Map* map, int neighbour, int tile) {
  // Catch wrong inputs
  if (tile < 0 || tile >= map->width * map->height) {
    return -1;
  }
  int offset = tile / map->width % 2 == 0 ? -1 : 1;
  // Fix revolution direction
  if (tile / map->width % 2 != 0) {
    switch (neighbour) {
      case 1:
        neighbour = 5;
        break;
      case 2:
        neighbour = 4;
        break;
      case 4:
        neighbour = 2;
        break;
      case 5:
        neighbour = 1;
        break;
    }
  }
  switch (neighbour) {
    case 0:
      return tile - map->width * 2 >= 0 ? tile - map->width * 2 : -1;
      break;
    case 1:
      return tile - map->width >= 0 ? tile - map->width : -1;
      break;
    case 2:
      return tile + map->width < map->width*map->height ? tile + map->width : -1;
      break;
    case 3:
      return tile + map->width * 2 < map->width*map->height ? tile + map->width * 2 : -1;
      break;
    case 4:
      return tile + map->width + offset < map->width*map->height ? tile + map->width + offset : -1;
      break;
    case 5:
      return tile - map->width + offset >= 0 ? tile - map->width + offset : -1;
      break;
    default:
      return -1;
  }
}
// Find amount of wood in neighbour tiles
int findMapTileNeighbourWood(Map* map, int tile) {
  int wood_count = 0;
  int neighbour = -1;
  for (int i = 0; i < 6; i ++) {
    neighbour = findMapTileNeighbour(map, i, tile);
    if (neighbour != -1) {
      wood_count += tileHasWood(map->tiles[neighbour]);
    }
  }
  return wood_count;
}
// Find number of crop fields in neighbour tiles
int findMapTileNeighbourCrops(Map* map, int tile) {
  int crop_count = 0;
  int neighbour = -1;
  for (int i = 0; i < 6; i ++) {
    neighbour = findMapTileNeighbour(map, i, tile);
    if (neighbour != -1 && map->tiles[neighbour]->type == GrassCrops) {
      crop_count ++;
    }
  }
  return crop_count;
}

// HELPER: Make uint power
uint intPow(int a, int n) {
  uint res = 1;
  for (int i = 0; i < n; i ++) {
    res *= a;
  }
  return res;
}
// HELPER: Find correct road
RoadType determineRoadType(RoadType current, int prev, int next) {
  int conn[4] = {0,0,0,0};
  // Map to 0-1-2-3
  prev -= prev > 3 ? 2 : 1;
  next -= next > 3 ? 2 : 1;
  // Store conn
  conn[prev] = 1;
  conn[next] = 1;
  // Add current conn
  switch (current) {
    case Cross:
      conn[0] = 1;
      conn[1] = 1;
      conn[2] = 1;
      conn[3] = 1;
      break;
    case BottomLeftTopRight:
      conn[0] = 1;
      conn[2] = 1;
      break;
    case TopLeftBottomRight:
      conn[1] = 1;
      conn[3] = 1;
      break;
    case TopLeftBottomLeft:
      conn[2] = 1;
      conn[3] = 1;
      break;
    case TopRightBottomRight:
      conn[0] = 1;
      conn[1] = 1;
      break;
    case BottomLeftBottomRight:
      conn[1] = 1;
      conn[2] = 1;
      break;
    case TopLeftTopRight:
      conn[0] = 1;
      conn[3] = 1;
      break;
    case BothLeftBottomRight:
      conn[1] = 1;
      conn[2] = 1;
      conn[3] = 1;
      break;
    case BottomLeftBothRight:
      conn[0] = 1;
      conn[1] = 1;
      conn[2] = 1;
      break;
    case BothLeftTopRight:
      conn[0] = 1;
      conn[2] = 1;
      conn[3] = 1;
      break;
    case TopLeftBothRight:
      conn[0] = 1;
      conn[1] = 1;
      conn[3] = 1;
      break;
    default:
      break;
  }
  // Determine road type
  if (conn[0] && conn[1] && conn[2] && conn[3]) {
    return Cross;
  } else if (!conn[0] && conn[1] && conn[2] && conn[3]) {
    return BothLeftBottomRight;
  } else if (conn[0] && !conn[1] && conn[2] && conn[3]) {
    return BothLeftTopRight;
  } else if (conn[0] && conn[1] && !conn[2] && conn[3]) {
    return TopLeftBothRight;
  } else if (conn[0] && conn[1] && conn[2] && !conn[3]) {
    return BottomLeftBothRight;
  } else if (!conn[0] && !conn[1] && conn[2] && conn[3]) {
    return TopLeftBottomLeft;
  } else if (!conn[0] && conn[1] && !conn[2] && conn[3]) {
    return TopLeftBottomRight;
  } else if (!conn[0] && conn[1] && conn[2] && !conn[3]) {
    return BottomLeftBottomRight;
  } else if (conn[0] && !conn[1] && !conn[2] && conn[3]) {
    return TopLeftTopRight;
  } else if (conn[0] && !conn[1] && conn[2] && !conn[3]) {
    return BottomLeftTopRight;
  } else if (conn[0] && conn[1] && !conn[2] && !conn[3]) {
    return TopRightBottomRight;
  }
  return None;
}
// Return is a road can be found that connects start->end
int findMapRoad(Map* map, int start, int end, int length) {
  // Test for water and invalid start tile
  if (
    start < 0 || start >= map->width * map->height || tileHasWater(map->tiles[start]) ||
    end < 0 || end >= map->width * map->height || tileHasWater(map->tiles[end])
  ) {
    return 0;
  }
  // Approximation
  length --;
  int distance_x = start % map->width - end % map->width;
  distance_x = distance_x > 0 ? distance_x : -distance_x;
  int distance_y = start / map->width - end / map->width;
  distance_y = distance_y > 0 ? distance_y : -distance_y;
  return distance_x * 2 + distance_y/2 < length;
  // Exact algorythm (to slow!)
  // int current_tile = start;
  // int previous_tile = start;
  // uint max = intPow(4, length);
  // // Test each possible path
  // for (uint i = 0; i < max; i ++) {
  //   // Test this path
  //   for (int j = 0, neighbour; j < length; j ++) {
  //     // Determine step direction
  //     neighbour = (int)(i / intPow(4, j)) % 4;
  //     neighbour += neighbour < 2 ? 1 : 2;
  //     neighbour = findMapTileNeighbour(map, neighbour, current_tile);
  //     if (neighbour == -1 || neighbour == previous_tile || tileHasWater(map->tiles[neighbour])) {
  //       // Invalid path, skip
  //       break;
  //     }
  //     // Set the new tiles and test if its the end
  //     previous_tile = current_tile;
  //     current_tile = neighbour;
  //     if (current_tile == end) {
  //       // Path is possible!
  //       return 1;
  //     }
  //   }
  //   // Reset path start
  //   current_tile = start;
  // }
  // return 0;
}
// Create a road on the map (NOTE: Currently permanent!)
int createMapRoad(Map* map, int start, int end, int length) {
  int best_distance = -1;
  uint best_path = 0;
  int current_tile = start;
  int previous_tile = start;
  uint max = intPow(4, length);
  // Test each possible path
  for (uint i = 0; i < max; i ++) {
    // Test this path
    for (int j = 0, neighbour; j < length; j ++) {
      // Determine step direction
      neighbour = (int)(i / intPow(4, j)) % 4;
      neighbour += neighbour < 2 ? 1 : 2;
      neighbour = findMapTileNeighbour(map, neighbour, current_tile);
      if (neighbour == -1 || neighbour == previous_tile || tileHasWater(map->tiles[neighbour])) {
        // Invalid path, skip
        break;
      }
      // Set the new tiles and test if its the end
      previous_tile = current_tile;
      current_tile = neighbour;
      if (current_tile == end && (best_distance > j || best_distance == -1)) {
        best_path = i;
        best_distance = j;
        break;
      }
    }
    // Reset path start
    current_tile = start;
  }
  // Test if a road was found
  if (best_distance == -1) {
    // There was no road
    return 0;
  }
  // Build the road
  current_tile = start;
  for (int j = 0, neighbour, next; j < best_distance; j ++) {
    // Determine step direction
    neighbour = (int)(best_path / intPow(4, j)) % 4;
    neighbour += neighbour < 2 ? 1 : 2;
    // Work out nest step direction
    next = (int)(best_path / intPow(4, j + 1)) % 4;
    next += next < 2 ? 1 : 2;
    // Place a road
    current_tile = findMapTileNeighbour(map, neighbour, current_tile);
    map->roads[current_tile] = determineRoadType(map->roads[current_tile], (neighbour + 3) % 6, next);
  }
  return 1;
}
