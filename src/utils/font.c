#include "font.h"

// Select a character (turns a char into a font position)
typedef struct {
  int y;
  int x;
} FontPosition;
FontPosition findCharPosition(char letter) {
  // The position to be returned
  FontPosition pos;
  // Determine the letter position
  if (letter >= 'A' && letter <= 'Z') {
    // A-Z
    pos.x = (1 + letter - 'A');
    pos.y = 2;
  } else if (letter >= 'a' && letter <= 'z') {
    // a-z
    pos.x = (1 + letter - 'a');
    pos.y = 3;
  } else if (letter >= '0' && letter <= '9') {
    // 0-9
    pos.x = (16 + letter - '0');
    pos.y = 1;
  } else {
    // Special chars
    switch (letter) {
      case ' ':
      case '\t':
      case '\r':
      case '\0':
        pos.x = 0;
        pos.y = 1;
        break;
      case '!':
        pos.x = 1;
        pos.y = 1;
        break;
      case '"':
        pos.x = 2;
        pos.y = 1;
        break;
      case '#':
        pos.x = 3;
        pos.y = 1;
        break;
      case '%':
        pos.x = 5;
        pos.y = 1;
        break;
      case '&':
        pos.x = 6;
        pos.y = 1;
        break;
      case '\'':
        pos.x = 7;
        pos.y = 1;
        break;
      case '(':
        pos.x = 8;
        pos.y = 1;
        break;
      case ')':
        pos.x = 9;
        pos.y = 1;
        break;
      case '*':
        pos.x = 10;
        pos.y = 1;
        break;
      case '+':
        pos.x = 11;
        pos.y = 1;
        break;
      case ',':
        pos.x = 12;
        pos.y = 1;
        break;
      case '-':
        pos.x = 13;
        pos.y = 1;
        break;
      case '.':
        pos.x = 14;
        pos.y = 1;
        break;
      case '/':
        pos.x = 15;
        pos.y = 1;
        break;
      case ':':
        pos.x = 26;
        pos.y = 1;
        break;
      case ';':
        pos.x = 27;
        pos.y = 1;
        break;
      case '<':
        pos.x = 28;
        pos.y = 1;
        break;
      case '=':
        pos.x = 29;
        pos.y = 1;
        break;
      case '>':
        pos.x = 30;
        pos.y = 1;
        break;
      case '?':
        pos.x = 31;
        pos.y = 1;
        break;
      case '[':
        pos.x = 27;
        pos.y = 2;
        break;
      case '\\':
        pos.x = 28;
        pos.y = 2;
        break;
      case ']':
        pos.x = 29;
        pos.y = 2;
        break;
      case '^':
        pos.x = 30;
        pos.y = 2;
        break;
      case '_':
        pos.x = 31;
        pos.y = 2;
        break;
      case '`':
        pos.x = 0;
        pos.y = 3;
        break;
      case '{':
        pos.x = 27;
        pos.y = 3;
        break;
      case '|':
        pos.x = 28;
        pos.y = 3;
        break;
      case '}':
        pos.x = 29;
        pos.y = 3;
        break;
      default:
        pos.x = 31;
        pos.y = 0;
    }
  }
  return pos;
}

// Create a font
Font* createFont(SDL_Renderer* renderer, int size, uint r, uint g, uint b) {
  // Allocate the font object
  Font* font = malloc(sizeof(Font));
  if (font == NULL) {
    return NULL;
  }
  // Load the font file (chars 4*6, file 32*4)
  SDL_Surface* font_bitmap = SDL_LoadBMP("assets/misc/font.bmp");
  font->letters = SDL_CreateTextureFromSurface(renderer, font_bitmap);
  if (font->letters == NULL) {
    SDL_FreeSurface(font_bitmap);
    free(font);
    return NULL;
  }
  SDL_SetTextureBlendMode(font->letters, SDL_BLENDMODE_BLEND);
  SDL_FreeSurface(font_bitmap);
  // Assign the renderer
  font->renderer = renderer;
  // Set the colors & size
  font->color_r = r;
  font->color_g = g;
  font->color_b = b;
  font->size = size;
  // We = done...
  return font;
}

// Destroy a font
void destroyFont(Font* font) {
  if (font == NULL) {
    return;
  }
  // Free the font texture
  SDL_DestroyTexture(font->letters);
  free(font);
}

// Render a string
void renderText(Font* font, SDL_Rect type_case, int char_count, char text[]) {
  SDL_Rect letter;
  letter.x = 0;
  letter.y = 0;
  letter.w = 4;
  letter.h = 6;
  SDL_Rect target;
  target.x = 0;
  target.y = 0;
  target.w = 4 * font->size;
  target.h = 6 * font->size;
  // Determine the amount of spaces and lines we have
  int places = type_case.w / target.w;
  int lines = type_case.h / target.h;
  // Vars needed in runtime
  int next_break;
  // Set the char color
  SDL_SetTextureColorMod(font->letters, font->color_r, font->color_g, font->color_b);
  // Print all the letters
  for (int i = 0, j = 0; j < char_count && i / places < lines; i ++, j++) {
    // Find the correct spot on the render target
    target.x = type_case.x + (i % places) * target.w;
    target.y = type_case.y + (i / places) * target.h;
    // On line break, skip the draw and skip to new line
    if (text[j] == '\n') {
      // The -1 accounts for the increment of the for loop
      i = (i / places + 1) * places - 1;
      continue;
    }
    // Determine if there should be an auto-break (at SPACE or -)
    next_break = 0;
    for (int k = 0; k < char_count - j; k ++, next_break ++) {
      // Test if the char is a breakable point
      if (text[j+k] == ' ' || text[j+k] == '-' || text[j+k] == '\n') {
        break;
      }
    }
    if (next_break > places - i % places && next_break <= places) {
      // The next word would overflow the box, break here
      i = (i / places + 1) * places - 1;
      j --; // Account for character not being printed here
      continue;
    }
    // If the character is a whitespace, it will be skipped at the beginning of the line
    if (text[j] == ' ' && i % places == 0) {
      i --; // Shift the writing position back to account for missing char
      continue;
    }
    // Determine the letter
    FontPosition letter_position = findCharPosition(text[j]);
    letter.y = letter.h * letter_position.y;
    letter.x = letter.w * letter_position.x;
    // Render the letter to the target position
    SDL_RenderCopy(font->renderer, font->letters, &letter, &target);
  }
}

// Render a string centered
// NOTE: This has it's own function, since this needs to iterate x2 (compared to simple rendering)
// TODO: This does not currently account for whitespace at the end/start of lines
void renderAlignedText(Font* font, FontAlignment alignment, SDL_Rect type_case, int char_count, char text[]) {
  SDL_Rect letter;
  letter.x = 0;
  letter.y = 0;
  letter.w = 4;
  letter.h = 6;
  SDL_Rect target;
  target.x = 0;
  target.y = 0;
  target.w = 4 * font->size;
  target.h = 6 * font->size;
  // Determine the amount of spaces and lines we have
  int places = type_case.w / target.w;
  int lines = type_case.h / target.h;
  // Vars for runtime
  int next_break;
  int line_lengths[lines];
  int line_offset;
  // Init the line lengths to 0
  for (int i = 0; i < lines; i ++) {
    line_lengths[i] = 0;
  }
  // Determine the number of chars in each line
  for (int i = 0, j = 0; j < char_count && i / places < lines; i ++, j++) {
    // On line break, skip the draw and skip to new line
    if (text[j] == '\n') {
      // The -1 accounts for the increment of the for loop
      i = (i / places + 1) * places - 1;
      continue;
    }
    // Determine if there should be an auto-break (at SPACE or -)
    next_break = 0;
    for (int k = 0; k < char_count - j; k ++, next_break ++) {
      // Test if the char is a breakable point
      if (text[j+k] == ' ' || text[j+k] == '-' || text[j+k] == '\n') {
        break;
      }
    }
    if (next_break > places - i % places && next_break <= places) {
      // The next word would overflow the box, break here
      i = (i / places + 1) * places - 1;
      j --; // Account for character not being printed here
      continue;
    }
    // The letter fits at this position, increment the line length
    line_lengths[i / places] ++;
  }
  // Set the char color
  SDL_SetTextureColorMod(font->letters, font->color_r, font->color_g, font->color_b);
  // Using the line lengths, render the characters
  for (int i = 0, l = 0; i < lines; i ++) {
    // If the line has no length, stop the loop
    if (line_lengths[i] == 0) {
      break;
    }
    // Determine line offset
    switch (alignment) {
      case CENTER:
        line_offset = (type_case.w - target.w * line_lengths[i]) / 2;
        break;
      case RIGHT:
        line_offset = (type_case.w - target.w * line_lengths[i]);
        break;
      case JUSTIFY:
        line_offset = (type_case.w - target.w * line_lengths[i]) / line_lengths[i];
        break;
      default:
        line_offset = 0;
    }
    for (int j = 0; j < line_lengths[i]; j ++, l ++) {
      // Ignore newline chars
      if (text[l] == '\n') {
        j --;
        continue;
      }
      // Here i is the line, j is the char pos in the line
      target.x = type_case.x + (alignment == JUSTIFY ? line_offset*j : line_offset) + j * target.w;
      target.y = type_case.y + i * target.h;
      // Determine the letter
      FontPosition letter_position = findCharPosition(text[l]);
      letter.y = letter.h * letter_position.y;
      letter.x = letter.w * letter_position.x;
      // Render the letter to the target position
      SDL_RenderCopy(font->renderer, font->letters, &letter, &target);
    }
  }
}
