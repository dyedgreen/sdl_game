#pragma once
#include "../frameworks.h"

// Font to be rendered
typedef struct {
  int size;
  uint color_r;
  uint color_g;
  uint color_b;
  SDL_Texture* letters;
  SDL_Renderer* renderer;
} Font;
typedef enum {
  LEFT,
  CENTER,
  RIGHT,
  JUSTIFY,
} FontAlignment;

// Create a font
Font* createFont(SDL_Renderer* renderer, int size, uint r, uint g, uint b);
// Destroy a font
void destroyFont(Font* font);
// Render a string
void renderText(Font* font, SDL_Rect type_case, int char_count, char text[]);
// Render a string with alignment (slower than renderText)
void renderAlignedText(Font* font, FontAlignment alignment, SDL_Rect type_case, int char_count, char text[]);

/**
* Supported Features:
*
* - No box overflow
* - Respect newline (\n)
* - Basic ASCII charset (pixel-font at 4*6 px)
* - Set size and font color
* - Trys to not break words
* - Support for basic alignment (slower, no
*   white-space management for auto line breaks)
*/
