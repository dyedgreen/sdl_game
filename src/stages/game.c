#import "game.h"

// Game constants
#define GAME_PLAYER_COUNT 2
#define GAME_CARDS_DRAW_COUNT 4
#define GAME_YEAR_ZERO 1431

// Function declarations for the game functions
void gameUpdate(Scene* scene, int frame);
void gameEventHandler(Scene* scene, SDL_Event* e);
int playCard(int card_type, void* game_data_payload);


// Main create game function
Stage* createGameStage(void (*replace)(int), SDL_Window* window, SDL_Renderer* renderer) {
  // Create the scene
  Stage* stage = createStage(&gameUpdate, &gameEventHandler, replace, window, renderer);
  if (stage == NULL) {
    return NULL;
  }
  // Allocate the game data
  stage->scene->data = malloc(sizeof(GameData));
  if (stage->scene->data == NULL) {
    destroyStage(stage);
    return NULL;
  }
  // Crate a temp pointer to game data
  GameData* game_data = (GameData*)stage->scene->data;
  // Do the game stage setup
  game_data->font = createFont(renderer, 2, 255, 0, 255); // DEBUG
  game_data->ui = createUi(renderer);
  game_data->theme = createTheme("assets/audio/curse_of_the_scarab.wav");
  game_data->hand = createHand(renderer, &playCard); // NOTE: Used by player and enemy!
  game_data->enemy_turn = createEnemyTurn();
  game_data->map = createMap(renderer, 40, 60);
  game_data->year = 0;
  game_data->story = createStory();
  if (
    game_data->font == NULL ||
    game_data->ui == NULL ||
    game_data->theme == NULL ||
    game_data->hand == NULL ||
    game_data->enemy_turn == NULL ||
    game_data->map == NULL ||
    game_data->story == NULL
  ) {
    destroyGameStage(stage);
    return NULL;
  }
  // Play music and sounds FIXME: Enable
  //pauseTheme(game_data->theme, 0);
  pauseHandSound(game_data->hand, 0);
  // Setup player hand NOTE: Do here?
  fillHand(game_data->hand, GAME_CARDS_DRAW_COUNT); // NOTE: Re-think
  drawCard(game_data->hand, DECK_CARD_END_TURN);
  // Return the finished game stage
  return stage;
}

void destroyGameStage(Stage* stage) {
  if (stage == NULL) {
    return;
  }
  GameData* game_data = (GameData*)stage->scene->data;
  // Free all memory occupied by the game scene
  destroyFont(game_data->font);
  destroyUi(game_data->ui);
  destroyTheme(game_data->theme);
  destroyHand(game_data->hand);
  destroyEnemyTurn(game_data->enemy_turn);
  destroyMap(game_data->map);
  destroyStory(game_data->story);
  // Free the game data
  free(stage->scene->data);
  // Destroy the Stage
  destroyStage(stage);
}


// Game Update Func
void gameUpdate(Scene* scene, int frame) {
  GameData* game_data = (GameData*)scene->data;
  /* INPUT STUFF */
  // Get the window bounds and keyboard / mouse inputs
  int window_w;
  int window_h;
  SDL_GetWindowSize(scene->window, &window_w, &window_h);
  const Uint8* keyboard = SDL_GetKeyboardState(NULL);
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  // Update the map scroll
  if ((keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_LEFT]) || mouse_x > window_w - 10) {
    scrollMap(game_data->map, 1, 0);
  } else if (keyboard[SDL_SCANCODE_LEFT] || mouse_x < 10) {
    scrollMap(game_data->map, -1, 0);
  }
  if ((keyboard[SDL_SCANCODE_UP] && !keyboard[SDL_SCANCODE_DOWN]) || mouse_y < 10) {
    scrollMap(game_data->map, 0, -1);
  } else if (keyboard[SDL_SCANCODE_DOWN] || mouse_y > window_h - 10) {
    scrollMap(game_data->map, 0, 1);
  }

  /* DETERMINE PLAYER TURN */
  int current_player = game_data->year % GAME_PLAYER_COUNT;
  // Perform enemy player turns
  if (current_player != 0) {
    // Advance the enemy player turn
    if (!advanceEnemyTurn(game_data->enemy_turn, game_data->hand, game_data)) {
      // Turn changes, play end of turn card (NOTE: Is card type 1)
      actionPlayCard(1, current_player, game_data);
      // If the human player is next, draw a new hand
      if ((current_player + 1) % GAME_PLAYER_COUNT == 0) {
        reshuffleHand(game_data->hand, GAME_CARDS_DRAW_COUNT);
        drawCard(game_data->hand, DECK_CARD_END_TURN);
      }
      // Reset the enemy turn (initial reset is done during allocation)
      resetEnemyTurn(game_data->enemy_turn, current_player + 1 == GAME_PLAYER_COUNT ? 1 : current_player + 1);
    }
  }

  /* RENDER STUFF */
  // Clear the window
  SDL_SetRenderDrawColor(scene->renderer, 255, 255, 255, 1);
  SDL_RenderClear(scene->renderer);
  // Render the map
  SDL_Rect map_target = {
    .w = window_w,
    .h = window_h,
    .x = 0,
    .y = 0,
  };
  renderMap(scene->renderer, game_data->map, map_target);
  // Render the ui
  game_data->ui->window.w = window_w;
  game_data->ui->window.h = window_h;
  renderUiQuest(
    game_data->ui, Warning,
    game_data->story->quests[game_data->story->current_quest].title,
    game_data->story->quests[game_data->story->current_quest].text,
    game_data->story->quests[game_data->story->current_quest].progress
  );
  SDL_Rect ui_year_position = {
    .x = scaleUiLength(game_data->ui, UI_QUEST_PADDING),
    .y = scaleUiLength(game_data->ui, UI_QUEST_PADDING)
  };
  char ui_year_text[] = "____\0";
  sprintf(ui_year_text, "%d", game_data->year + GAME_YEAR_ZERO);
  renderUiLabel(game_data->ui, ui_year_position, ui_year_text);
  // Render the hand
  SDL_Rect hand_target = {
    .w = window_w,
    .h = window_h / 6,
    .x = 0,
    .y = window_h * 5 / 6,
  };
  renderHand(scene->renderer, game_data->hand, hand_target, frame);
  // Render the enemy turn TODO (figure out ui + set target)
  if (current_player != 0) {
    renderEnemyTurn(scene->renderer, game_data->enemy_turn, game_data->hand, map_target, frame);
  }

  // Highlight the current tile, if card is selected
  if (game_data->hand->card_selected != -1) {
    if (actionCanPlayCard(game_data->hand->deck->cards[game_data->hand->card_selected]->type, 0, game_data)) {
      game_data->map->highlight_r = 255;
      game_data->map->highlight_g = 255;
      game_data->map->highlight_b = 255;
    } else {
      game_data->map->highlight_r = 255;
      game_data->map->highlight_g = 0;
      game_data->map->highlight_b = 0;
    }
    game_data->map->tile_highlighted = game_data->map->tile_hovered;
  } else if (game_data->map->tiles[game_data->map->tile_hovered]->player != -1) {
    int r = 0, g = 0, b = 0;
    // Supports 4 players! TODO: This can be more elegant!
    switch (game_data->map->tiles[game_data->map->tile_hovered]->player) {
      case 0:
        b = 255;
        break;
      case 1:
        g = 255;
        break;
      case 2:
        r = 255;
        b = 255;
        break;
      default:
        g = 255;
        b = 255;
        break;
    }
    game_data->map->highlight_r = r;
    game_data->map->highlight_g = g;
    game_data->map->highlight_b = b;
    game_data->map->tile_highlighted = game_data->map->tile_hovered;
  } else {
    game_data->map->tile_highlighted = -1;
  }

  /* AUDIO STUFF */
  // Play the theme music on loop
  loopTheme(game_data->theme);

  /* THESE ARE TESTS / DEBUG STUFF */
  // Render some test ui elements
  // SDL_Rect ui_test = { .w = 0, .h = 0, .x = 100, .y = 200 };
  // char* ui_text = "Play\0";
  // game_data->ui->window.w = window_w;
  // game_data->ui->window.h = window_h;
  // renderUiBigButton(game_data->ui, ui_test, Select, ui_text);
  // ui_test.x += 200;
  // ui_test.w = 300;
  // renderUiProgress(game_data->ui, ui_test, Blue, 100 * frame / 60);
  // ui_test.h = 250;
  // ui_test.x += 310;
  // ui_test = renderUiBox(game_data->ui, ui_test);
  // renderUiIcon(game_data->ui, ui_test, Warning);
  // Set surrounding tiles to be highlighted
  // game_data->map->tile_highlighted = findMapTileNeighbour(game_data->map, frame / 10 % 6, game_data->map->tile_hovered);
  // game_data->map->tile_highlighted = game_data->map->tile_hovered;
  // Render max outline
  SDL_SetRenderDrawColor(scene->renderer, 255, 0, 255, 1);
  //SDL_RenderDrawRect(scene->renderer, &map_target);
  // Draw some debug information
  SDL_Rect text_box = {
    .w = 4 * 2 * 30,
    .h = 6 * 2 * 5,
    .x = 5,
    .y = window_h - 6 * 2 * 5 - 5,
  };
  int current_tile = game_data->map->tile_hovered;
  char text[] = "Current tile: _____\n     (frame: __, year:____,\nplayer: _)";
  text[14] = '0' + current_tile / 10000 % 10;
  text[15] = '0' + current_tile / 1000 % 10;
  text[16] = '0' + current_tile / 100 % 10;
  text[17] = '0' + current_tile / 10 % 10;
  text[18] = '0' + current_tile % 10;
  text[33] = '0' + frame / 10 % 10;
  text[34] = '0' + frame % 10;
  text[42] = '0' + game_data->year / 1000 % 10;
  text[43] = '0' + game_data->year / 100 % 10;
  text[44] = '0' + game_data->year / 10 % 10;
  text[45] = '0' + game_data->year % 10;
  text[56] = '0' + current_player % 10;
  SDL_SetRenderDrawColor(scene->renderer, 255, 0, 255, 1);
  SDL_RenderDrawRect(scene->renderer, &text_box);
  renderText(game_data->font, text_box, sizeof(text) / sizeof(char), text);

  /* FINAL RENDER PRESENTATION */
  // Draw/Present the renderer
  SDL_RenderPresent(scene->renderer);
}


// Game Event Handler
void gameEventHandler(Scene* scene, SDL_Event* e) {
  GameData* game_data = (GameData*)scene->data;
  /* DISCRETE INPUTS (not frame by frame) */
  // Update the hand
  updateHand(game_data->hand, e, game_data);

  /* TESTS AND DEBUG STUFF */
  if (e->type == SDL_KEYUP) {
    if (((SDL_KeyboardEvent*)e)->keysym.scancode == SDL_SCANCODE_SPACE) {
      drawCards(((GameData*)scene->data)->hand, 1);
    } else if (((SDL_KeyboardEvent*)e)->keysym.scancode == SDL_SCANCODE_A) {
      // Play/pause to not annoy anyone else
      pauseTheme(game_data->theme, SDL_GetAudioDeviceStatus(game_data->theme->device) == SDL_AUDIO_PLAYING ? 1 : 0);
    }
  }
}

// Play card handler
int playCard(int card_type, void* game_data_payload) {
  // Execute the play action NOTE: The actual player is p0!
  return actionPlayCard(card_type, 0, game_data_payload);
}
