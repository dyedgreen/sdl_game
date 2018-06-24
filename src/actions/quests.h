#pragma once
#include "../frameworks.h"

#define QUESTS_MAX_PROGRESS 100
#define QUESTS_STORY_LENGTH 5 // How many quests to do in a full game

typedef struct {
  char* title;
  char* text;
  int progress;
  int id;
} Quest;

typedef struct {
  Quest quests[QUESTS_STORY_LENGTH];
  int current_quest;
} Story;

Story* createStory();
void destroyStory(Story* story);

// TODO: Evaluate quest progress from game data