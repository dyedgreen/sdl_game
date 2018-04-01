#import "card.h"

#define CARD_WIDTH 120
#define CARD_HEIGHT 180


Card* createCard(SDL_Renderer* renderer, char* picture, int anim_speed, char* title, char* body, int type, int cost, int sticky) {
  // Create the card object
  Card* card = malloc(sizeof(Card));
  if (card == NULL) {
    return NULL;
  }
  // Copy all the cards information
  card->type = type;
  card->cost = cost;
  card->sticky = sticky;
  card->title_len = 0;
  card->body_len = 0;
  // Copy the cards title and body
  for (int i = 0; title[i] != '\0' && i < lengthof(card->title); i ++) {
    card->title[i] = title[i];
    card->title_len ++;
  }
  for (int i = 0; body[i] != '\0' && i < lengthof(card->body); i ++) {
    card->body[i] = body[i];
    card->body_len ++;
  }
  // Create the title and body fonts
  Font* title_font = createFont(renderer, 2, 238, 232, 124);
  Font* body_font = createFont(renderer, 1, 255, 175, 60);
  // Load the card picture and back texture
  SDL_Surface* card_picture_bitmap = SDL_LoadBMP(picture);
  SDL_Texture* card_picture_texture = SDL_CreateTextureFromSurface(renderer, card_picture_bitmap);
  SDL_Surface* card_back_bitmap = SDL_LoadBMP("assets/misc/card_dark.bmp");
  SDL_Texture* card_back_texture = SDL_CreateTextureFromSurface(renderer, card_back_bitmap);
  SDL_Texture* card_target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, card_picture_bitmap->w, card_picture_bitmap->h);
  // Test if everything could be created
  if (title_font == NULL || body_font == NULL || card_picture_bitmap == NULL || card_picture_texture == NULL || card_back_texture == NULL || card_target_texture == NULL) {
    SDL_FreeSurface(card_picture_bitmap);
    SDL_FreeSurface(card_back_bitmap);
    SDL_DestroyTexture(card_picture_texture);
    SDL_DestroyTexture(card_back_texture);
    SDL_DestroyTexture(card_target_texture);
    destroyFont(title_font);
    destroyFont(body_font);
    free(card);
    return NULL;
  }
  // Create a sprite (uses a card picture)
  card->sprite = createSprite(renderer, picture, card_picture_bitmap->w / CARD_WIDTH, anim_speed);
  if (card->sprite == NULL) {
    SDL_FreeSurface(card_picture_bitmap);
    SDL_FreeSurface(card_back_bitmap);
    SDL_DestroyTexture(card_picture_texture);
    SDL_DestroyTexture(card_back_texture);
    SDL_DestroyTexture(card_target_texture);
    destroyFont(title_font);
    destroyFont(body_font);
    free(card);
    return NULL;
  }
  SDL_FreeSurface(card_picture_bitmap);
  SDL_FreeSurface(card_back_bitmap);
  // Load the card back and assemble the card on the sprite texture
  SDL_SetRenderTarget(renderer, card_target_texture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_Rect src_rect = {
    .w = CARD_WIDTH,
    .h = CARD_HEIGHT,
    .x = 0,
    .y = 0,
  };
  SDL_Rect target_rect = src_rect;
  // SDL_Rect title_box_rect = {.w = 100, .h = 16, .x = 10, .y = 9}; // Light
  SDL_Rect title_box_rect = {.w = 100, .h = 16, .x = 10, .y = 11}; // Dark
  SDL_Rect body_box_rect = {.w = 100, .h = 60, .x = 10, .y = 101};
  // SDL_Rect cost_box_rect = {.w = 20, .h = 16, .x = 99, .y = 156}; // Light
  SDL_Rect cost_box_rect = {.w = 20, .h = 16, .x = 97, .y = 149}; // Dark
  char cost_string[1] = {'0'+card->cost};
  // Render the card backs and text
  for (int i = 0; i < card->sprite->frame_count; i ++) {
    SDL_RenderCopy(renderer, card_back_texture, &src_rect, &target_rect);
    renderAlignedText(title_font, CENTER, title_box_rect, card->title_len, card->title);
    renderText(body_font, body_box_rect, card->body_len, card->body);
    renderText(title_font, cost_box_rect, 1, cost_string);
    target_rect.x += CARD_WIDTH;
    title_box_rect.x += CARD_WIDTH;
    body_box_rect.x += CARD_WIDTH;
    cost_box_rect.x += CARD_WIDTH;
  }
  // Render the picture
  src_rect.w = CARD_WIDTH * card->sprite->frame_count;
  src_rect.h = CARD_HEIGHT;
  src_rect.x = 0;
  src_rect.y = 0;
  target_rect = src_rect;
  SDL_RenderCopy(renderer, card_picture_texture, &src_rect, &target_rect);
  // Clean up everything
  SDL_SetRenderTarget(renderer, NULL);
  SDL_DestroyTexture(card_picture_texture);
  SDL_DestroyTexture(card_back_texture);
  SDL_DestroyTexture(card->sprite->texture);
  card->sprite->texture = card_target_texture;
  destroyFont(title_font);
  destroyFont(body_font);
  // Return the finished card
  return card;
}

void destroyCard(Card* card) {
  if (card == NULL) {
    return;
  }
  // Destroy the sprite
  destroySprite(card->sprite);
  free(card);
}

// This will render a card destructing by play
int renderCardPlay(SDL_Renderer* renderer, Card* card, int frame, SDL_Point* target_point) {
  // TODO
  return 1;
}
