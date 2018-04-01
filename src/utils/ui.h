#pragma once
#include "font.h"
#include "../frameworks.h"

#define UI_MAX_PROGRESS 100

#define UI_QUEST_WIDTH 160
#define UI_QUEST_PADDING 5
#define UI_QUEST_PADDING_OUTER 4

typedef struct {
  SDL_Renderer* renderer;
  SDL_Rect window;
  SDL_Texture* pices;
  Font* font;
} Ui;

typedef enum {
  Play,
  Select,
  Close,
} UiBigButtonIcon;

typedef enum {
  Blue,
  Red,
  Green,
} UiProgressColor;

typedef enum {
  Info,
  Message,
  Warning,
  Error,
} UiIcon;

Ui* createUi(SDL_Renderer* renderer);
void destroyUi(Ui* ui);

// Basic Ui Elements
SDL_Rect renderUiBigButton(Ui* ui, SDL_Rect position, UiBigButtonIcon icon, char* text); // TODO: Hover effect...
void renderUiProgress(Ui* ui, SDL_Rect position, UiProgressColor color, int progress);
void renderUiLabel(Ui* ui, SDL_Rect position, char* text);
SDL_Rect renderUiBox(Ui* ui, SDL_Rect position);
SDL_Rect renderUiIcon(Ui* ui, SDL_Rect position, UiIcon icon);

// High-Level Ui Elements
void renderUiQuest(Ui* ui, UiIcon icon, char* title, char* text, int progress);

// Outside helper functions
int scaleUiLength(Ui* ui, int length);
