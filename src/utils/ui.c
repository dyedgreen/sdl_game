#include "ui.h"

#define UI_RENDER_SCALE_FACTOR 600
#define UI_RENDER_SCALE (ui->window.w < UI_RENDER_SCALE_FACTOR ? 1 : ui->window.w / UI_RENDER_SCALE_FACTOR)


Ui* createUi(SDL_Renderer* renderer) {
  Ui* ui = malloc(sizeof(Ui));
  if (ui == NULL) {
    return NULL;
  }
  // Get font
  ui->font = createFont(renderer, 1, 255, 255, 255);
  if (ui->font == NULL) {
    free(ui);
    return NULL;
  }
  // Get ui pices
  SDL_Surface* bitmap = SDL_LoadBMP("assets/misc/ui.bmp");
  ui->pices = SDL_CreateTextureFromSurface(renderer, bitmap);
  SDL_FreeSurface(bitmap);
  if (ui->pices == NULL) {
    destroyFont(ui->font);
    free(ui);
    return NULL;
  }
  ui->renderer = renderer;
  SDL_Rect window = { .w = 0, .h = 0, .x = 0, .y = 0 };
  ui->window = window;
  return ui;
}

void destroyUi(Ui* ui) {
  if (ui == NULL) {
    return;
  }
  destroyFont(ui->font);
  SDL_DestroyTexture(ui->pices);
  free(ui);
}

// Basic render operations
SDL_Rect renderUiBigButton(Ui* ui, SDL_Rect position, UiBigButtonIcon icon, char* text) {
  SDL_Rect full = { .w = 96 * UI_RENDER_SCALE, .h = 32 * UI_RENDER_SCALE, .x = position.x, .y = position.y };
  SDL_Rect src_icon = { .w = 25, .h = 32, .x = 115, .y = 58 };
  SDL_Rect src_button = { .w = 71, .h = 26, .x = 121, .y = 96 };
  // Determine icon
  switch (icon) {
    case Play:
      // Is default
      break;
    case Select:
      src_icon.x += 26;
      break;
    case Close:
      src_icon.x += 52;
      break;
    default:
      // Is play
      break;
  }
  // Render the icon
  position.w = src_icon.w * UI_RENDER_SCALE;
  position.h = src_icon.h * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_icon, &position);
  // Render the button
  position.x += position.w;
  position.y += 3 * UI_RENDER_SCALE;
  position.w = src_button.w * UI_RENDER_SCALE;
  position.h = src_button.h * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_button, &position);
  // Render the button text
  position.w -= 10 * UI_RENDER_SCALE;
  position.h -= 14 * UI_RENDER_SCALE;
  position.x += 4 * UI_RENDER_SCALE;
  position.y += 8 * UI_RENDER_SCALE;
  ui->font->size = 2 * UI_RENDER_SCALE;
  int size;
  for (size = 0;size < 16 && text[size] != '\0'; size++) {}
  renderAlignedText(ui->font, CENTER, position, size, text);
  // Return button final bounds
  return full;
}

void renderUiProgress(Ui* ui, SDL_Rect position, UiProgressColor color, int progress) {
  // Cap progress
  progress = progress > UI_MAX_PROGRESS ? UI_MAX_PROGRESS : progress;
  // Determine length of middle bit
  int length = position.w - 20 * UI_RENDER_SCALE;
  SDL_Rect src_bg = { .w = 10, .h = 14, .x = 33, .y = 75 };
  SDL_Rect src_fill = { .w = 2, .h = 8, .x = 65, .y = 41 };
  // Determine color
  switch (color) {
    case Blue:
      // Default
      break;
    case Red:
      src_fill.y += 9;
      break;
    case Green:
      src_fill.y += 18;
      break;
    default:
      // Blue
      break;
  }
  // Render left cap
  position.w = 10 * UI_RENDER_SCALE;
  position.h = 14 * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_bg, &position);
  // Render bar
  src_bg.w = 2;
  src_bg.x += 11;
  position.w = 2 * UI_RENDER_SCALE;
  position.x += 10 * UI_RENDER_SCALE;
  int i;
  for (i = 0; i * 2 * UI_RENDER_SCALE < length; i ++) {
    SDL_RenderCopy(ui->renderer, ui->pices, &src_bg, &position);
    position.x += 2 * UI_RENDER_SCALE;
  }
  // Render right cap
  src_bg.w = 10;
  src_bg.x += 13;
  position.w = 10 * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_bg, &position);
  // Render fill
  position.h = 8 * UI_RENDER_SCALE;
  position.w = 2 * UI_RENDER_SCALE;
  position.x -= i * 2 * UI_RENDER_SCALE;
  position.y += 3 * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_fill, &position);
  src_fill.x += 3;
  for (int j = 0; j < (i - 1) * progress  / UI_MAX_PROGRESS; j ++) {
    position.x += 2 * UI_RENDER_SCALE;
    SDL_RenderCopy(ui->renderer, ui->pices, &src_fill, &position);
  }
  src_fill.x += 7;
  SDL_RenderCopy(ui->renderer, ui->pices, &src_fill, &position);
}

void renderUiLabel(Ui* ui, SDL_Rect position, char* text) {
  // Find text length
  int len;
  for (len = 0; len < 128 && text[len] != '\0'; len ++) {}
  // If the width is given as 0, it is scaled to fit the text
  int width;
  if (position.w == 0) {
    width = len * 4 * UI_RENDER_SCALE;
    position.w = width + 45 * UI_RENDER_SCALE;
  } else {
    width = position.w - 45 * UI_RENDER_SCALE;
  }
  SDL_Rect src = { .w = 12, .h = 18, .x = 79, .y = 38 };
  SDL_Rect target = { .w = 12 * UI_RENDER_SCALE, .h = 18 * UI_RENDER_SCALE, .x = position.x, .y = position.y };
  // Render the left and right caps
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  src.w = 33;
  src.x = 103;
  target.w = 33 * UI_RENDER_SCALE;
  target.x += width + 12 * UI_RENDER_SCALE;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  // Render fill
  src.w = 1;
  src.x = 92;
  target.w = width;
  target.x -= width;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  // Render the text
  position.w -= 24 * UI_RENDER_SCALE;
  position.h = 12 * UI_RENDER_SCALE;
  position.x += 12 * UI_RENDER_SCALE;
  position.y += 4 * UI_RENDER_SCALE;
  ui->font->size = 2 * UI_RENDER_SCALE;
  renderAlignedText(ui->font, CENTER, position, len, text);
}

SDL_Rect renderUiBox(Ui* ui, SDL_Rect position) {
  int width = position.w - 19 * UI_RENDER_SCALE;
  int height = position.h - 14 * UI_RENDER_SCALE;
  SDL_Rect target = position;
  SDL_Rect src = { .w = 0, .h = 0, .x = 0, .y = 0 };
  // Render Corners
  target.w = 10 * UI_RENDER_SCALE;
  target.h = 7 * UI_RENDER_SCALE;
  src.w = 10;
  src.h = 7;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  target.y += height + 7 * UI_RENDER_SCALE;
  src.y = 32;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  target.x += width + 10 * UI_RENDER_SCALE;
  src.w = 9;
  src.x = 69;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  target.y -= height + 7 * UI_RENDER_SCALE;
  src.y = 0;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  // Render top and bottom borders
  target.w = 57 * UI_RENDER_SCALE;
  target.h = 7 * UI_RENDER_SCALE;
  target.x = position.x + 10 * UI_RENDER_SCALE;
  target.y = position.y;
  src.w = 57;
  src.h = 7;
  src.x = 11;
  src.y = 0;
  int i;
  for (i = 0; i < width / UI_RENDER_SCALE / 57; i ++) {
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.y += height + 7 * UI_RENDER_SCALE;
    src.y = 32;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.y -= height + 7 * UI_RENDER_SCALE;
    src.y = 0;
    target.x += 57 * UI_RENDER_SCALE;
  }
  if (i * 57 * UI_RENDER_SCALE < width) {
    target.w = width - i * 57 * UI_RENDER_SCALE;
    src.w = target.w / UI_RENDER_SCALE;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.y += height + 7 * UI_RENDER_SCALE;
    src.y = 32;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  }
  // Render left and right borders
  target.w = 10 * UI_RENDER_SCALE;
  target.h = 23 * UI_RENDER_SCALE;
  target.x = position.x;
  target.y = position.y + 7 * UI_RENDER_SCALE;
  src.w = 10;
  src.h = 23;
  src.x = 0;
  src.y = 8;
  for (i = 0; i < height / UI_RENDER_SCALE / 23; i ++) {
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.x += width + 10 * UI_RENDER_SCALE;
    src.x = 68;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.x -= width + 10 * UI_RENDER_SCALE;
    src.x = 0;
    target.y += 23 * UI_RENDER_SCALE;
  }
  if (i * 23 * UI_RENDER_SCALE < height) {
    target.h = height - i * 23 * UI_RENDER_SCALE;
    src.h = target.h / UI_RENDER_SCALE;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
    target.x += width + 10 * UI_RENDER_SCALE;
    src.x = 68;
    SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  }
  // Render Center
  target.w = width + 8 * UI_RENDER_SCALE;
  target.h = height + 2 * UI_RENDER_SCALE;
  target.x = position.x + 6 * UI_RENDER_SCALE;
  target.y = position.y + 6 * UI_RENDER_SCALE;
  src.w = 1;
  src.h = 1;
  src.x = 11;
  src.y = 8;
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &target);
  return target;
}

SDL_Rect renderUiIcon(Ui* ui, SDL_Rect position, UiIcon icon) {
  SDL_Rect src = { .w = 16, .h = 16, .x = 0, .y = 0 };
  position.w = 16 * UI_RENDER_SCALE;
  position.h = 16 * UI_RENDER_SCALE;
  switch (icon) {
    case Info:
      src.x = 79;
      src.y = 57;
      break;
    case Message:
      src.x = 96;
      src.y = 57;
      break;
    case Warning:
      src.x = 79;
      src.y = 74;
      break;
    case Error:
    default:
      src.x = 96;
      src.y = 74;
      break;
  }
  SDL_RenderCopy(ui->renderer, ui->pices, &src, &position);
  return position;
}


// Combined render operations
void renderUiQuest(Ui* ui, UiIcon icon, char* title, char* text, int progress) {
  // Find title and text char counts
  int length_title;
  int length_text;
  for (length_title = 0;length_title < 64 && title[length_title] != '\0'; length_title++) {}
  for (length_text = 0;length_text < 2048 && text[length_text] != '\0'; length_text++) {}
  // Determine required box size
  SDL_Rect box = { .w = UI_QUEST_WIDTH * UI_RENDER_SCALE };
  int text_width = box.w - (UI_QUEST_PADDING + 2 * UI_QUEST_PADDING_OUTER + 16) * UI_RENDER_SCALE;
  box.h = 2 * UI_QUEST_PADDING_OUTER * UI_RENDER_SCALE + 14 * UI_RENDER_SCALE;
  box.h += 16 * UI_RENDER_SCALE;
  ui->font->size = UI_RENDER_SCALE;
  box.h += renderTextRequiredHeight(ui->font, text_width, length_text + length_text / 8);
  box.h += UI_QUEST_PADDING * UI_RENDER_SCALE;
  // Determine box position
  box.x = ui->window.w - UI_QUEST_PADDING * UI_RENDER_SCALE - box.w;
  box.y = UI_QUEST_PADDING * UI_RENDER_SCALE;
  // Render box
  SDL_Rect box_inner = renderUiBox(ui, box);
  // Render Icon
  box_inner.x += UI_QUEST_PADDING_OUTER * UI_RENDER_SCALE;
  box_inner.y += UI_QUEST_PADDING_OUTER * UI_RENDER_SCALE;
  SDL_Rect icon_pos = renderUiIcon(ui, box_inner, icon);
  // Render title and text
  SDL_Rect type_case = {
    .w = box_inner.w - icon_pos.w - (2 * UI_QUEST_PADDING_OUTER + UI_QUEST_PADDING + 2) * UI_RENDER_SCALE,
    .h = 12 * UI_RENDER_SCALE,
    .x = icon_pos.x + icon_pos.w + UI_QUEST_PADDING * UI_RENDER_SCALE,
    .y = box_inner.y + 2 * UI_RENDER_SCALE,
  };
  ui->font->size = 2 * UI_RENDER_SCALE;
  renderText(ui->font, type_case, length_title, title);
  type_case.h = box_inner.h - icon_pos.h - 2 * UI_QUEST_PADDING_OUTER * UI_RENDER_SCALE;
  type_case.y += 14 * UI_RENDER_SCALE;
  ui->font->size = UI_RENDER_SCALE;
  renderText(ui->font, type_case, length_text, text);
  // Render progress
  box.y += box.h + UI_QUEST_PADDING * UI_RENDER_SCALE;
  renderUiProgress(ui, box, Blue, progress);
}

// Helper functions
int scaleUiLength(Ui* ui, int length) {
  return length * UI_RENDER_SCALE;
}
