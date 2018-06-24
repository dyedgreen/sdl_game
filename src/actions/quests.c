#include "quests.h"

#define QUESTS_MAX_QUEST_ID 0

void fillQuest(Quest* quest, int id);

Story* createStory() {
  Story* story = malloc(sizeof(Story));
  if (story == NULL) {
    return NULL;
  }
  int taken[QUESTS_STORY_LENGTH];
  // Fill in each quest
  for (int i = 0; i < QUESTS_STORY_LENGTH; i ++) {
    // Get a pseudo-random quest
    // TODO / DEBUG
    fillQuest(&story->quests[i], 0);
  }
  return story;
}

void destroyStory(Story* story) {
  if (story == NULL) {
    return;
  }
  free(story);
  // NOTE: This doesn't free the strings, as they are static and not allocated at runtime.
}

void fillQuest(Quest* quest, int id) {
  quest->id = id;
  switch (id) {
    // DEBUG QUEST
    case 0:
      quest->title = "Test Quest";
      quest->text = "The progress system is still WIP...";
      quest->progress = 0;
      break;
    // CATCH INVALID IDS
    default:
      quest->id = -1;
      quest->title = "Error";
      quest->text = "This quest does not exist.";
      break;
  }
}