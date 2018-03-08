#import "hand.h"

// Render constants
#define HAND_RENDER_SCALE_FACTOR 600
#define HAND_MAX_TILT 15
#define HAND_MAX_TOP 20
#define HAND_OFFSET 80
#define HAND_HOVER_ANIM_SPEED 1
#define HAND_REMOVE_ANIM_SPEED 1
#define HAND_FOLLOW_SPEED 1
#define HAND_FOLLOW_OFFSET 10


Hand* createHand(SDL_Renderer* renderer, int (*play)(int, void*)) {
  // Allocate the hand object
  Hand* hand = malloc(sizeof(Hand));
  if (hand == NULL) {
    return NULL;
  }
  // Allocate the deck
  hand->deck = createDeck(renderer);
  if (hand->deck == NULL) {
    free(hand);
    return NULL;
  }
  hand->play = play;
  // Load the sound effects
  if (
    SDL_LoadWAV("assets/audio/card_draw.wav", &hand->wav_spec[0], &hand->wav_buffer[0], &hand->wav_length[0]) == NULL ||
    SDL_LoadWAV("assets/audio/card_place.wav", &hand->wav_spec[1], &hand->wav_buffer[1], &hand->wav_length[1]) == NULL ||
    SDL_LoadWAV("assets/audio/card_shove.wav", &hand->wav_spec[2], &hand->wav_buffer[2], &hand->wav_length[2]) == NULL
  ) {
    // Failed to load audio
    destroyHand(hand);
    return NULL;
  }
  // Open the audio device
  hand->wav_spec[0].callback = NULL;
  hand->audio_device = SDL_OpenAudioDevice(NULL, 0, &hand->wav_spec[0], &hand->audio_device_spec, 0);
  if (hand->audio_device == 0) {
    // Failed to open device
    destroyHand(hand);
    return NULL;
  }
  // Set all initial values
  hand->card_hovered = -1;
  hand->card_selected = -1;
  // Set all the rendering stuff
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    hand->card_hover_progress[i] = 0;
    hand->card_remove_progress[i] = 0;
    hand->card_follow_distance[i] = 100;
  }
  // Return the finished hand
  return hand;
}

void destroyHand(Hand* hand) {
  if (hand == NULL) {
    return;
  }
  // Destroy the deck
  destroyDeck(hand->deck);
  // Destroy the audio
  SDL_FreeWAV(hand->wav_buffer[0]);
  SDL_FreeWAV(hand->wav_buffer[1]);
  SDL_FreeWAV(hand->wav_buffer[2]);
  // Close the device
  SDL_CloseAudioDevice(hand->audio_device);
  // Free the hand
  free(hand);
}


// Private helper functions
int getHoverProgressOfNextCard(Hand* hand, int current, int left) {
  if (left) {
    // Look left
    for (int i = current - 1; i + 1 > 0; i --) {
      if (hand->deck->card_used[i]) {
        return hand->card_hover_progress[i];
      }
    }
  } else {
    // Look right
    for (int i = current + 1; i < lengthof(hand->card_hover_progress); i ++) {
      if (hand->deck->card_used[i]) {
        return hand->card_hover_progress[i];
      }
    }
  }
  // Non found
  return 0;
}
int getMinFollowDistance(Hand* hand) {
  int distance = 100;
  for (int i = 0; i < lengthof(hand->card_follow_distance); i ++) {
    if (hand->card_follow_distance[i] < distance) {
      distance = hand->card_follow_distance[i];
    }
  }
  return distance;
}

typedef enum {
  Draw = 0,
  Place = 1,
  Shove = 2,
} HandSoundEffect;
void playSoundEffect(Hand* hand, HandSoundEffect effect) {
  // Determine if device is paused
  if (SDL_GetAudioDeviceStatus(hand->audio_device) == SDL_AUDIO_PLAYING) {
    SDL_QueueAudio(hand->audio_device, hand->wav_buffer[effect], hand->wav_length[effect]);
  }
}

// Render the hand into the target, obeying all rules set by the current state
void renderHand(SDL_Renderer* renderer, Hand* hand, SDL_Rect target, int frame) {
  // Determine the hand render scale
  float hand_render_scale = target.w / HAND_RENDER_SCALE_FACTOR;
  // Find length of hand and total neg hover progress
  int card_count = 0;
  int removed_card_count = 0;
  int neg_hover_progress = 0; // positive value (!)
  int card_hovered = -1; // To store the hovored card
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    if (hand->deck->card_used[i]) {
      card_count ++;
      if (hand->card_hover_progress[i] < 0) {
        neg_hover_progress -= hand->card_hover_progress[i];
      }
    } else if (hand->card_remove_progress[i] > 0) {
      // Account for remove progress (non 0 -> card is being removed)
      card_count ++;
      removed_card_count ++; // To adjust rotation
      neg_hover_progress += hand->card_remove_progress[i];
    }
  }

  // Basic render target for non-disturbed situation
  SDL_Rect render_target = {
    .w = hand->deck->cards[0]->sprite->w * hand_render_scale,
    .h = hand->deck->cards[0]->sprite->h * hand_render_scale,
    // Center of target - half hand width - half a card + card width taken by hidden cards
    .x = target.x + target.w / 2 - card_count / 2 * HAND_OFFSET * hand_render_scale - hand->deck->cards[0]->sprite->w / 2 * hand_render_scale + neg_hover_progress * HAND_OFFSET * hand_render_scale / 200,
    .y = target.y + HAND_MAX_TOP * hand_render_scale,
  };
  // Dynamic render target that moves based on render state
  SDL_Rect d_render_target;
  // Shift to right for even number of cards
  if (card_count % 2 == 0) {
    render_target.x += hand->deck->cards[0]->sprite->w / 3 * hand_render_scale;
  }

  // Get the position of the mouse
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);

  // Render each card
  for (int i = 0, j = 0; i < lengthof(hand->deck->cards) && j < card_count; i ++) {
    // If card is in hand, render and inc. j
    if (hand->deck->card_used[i] || hand->card_remove_progress[i] > 0) {
      // Rotate the card
      if (card_count != removed_card_count) {
        hand->deck->cards[i]->sprite->rotation = 2.0 * (float)HAND_MAX_TILT * ((float)j / (float)(card_count-removed_card_count)) - (float)HAND_MAX_TILT;
      } else {
        hand->deck->cards[i]->sprite->rotation = 2.0 * (float)HAND_MAX_TILT * ((float)j / (float)(card_count)) - (float)HAND_MAX_TILT;
      }

      // Account for remove progress
      if (hand->card_remove_progress[i] > 0) {
        hand->card_hover_progress[i] = - hand->card_remove_progress[i];
      }

      // If neigbour is hovered, shif away
      d_render_target = render_target;
      if (getHoverProgressOfNextCard(hand, i, 1) > 0) {
        // Shift to right
        d_render_target.x += getHoverProgressOfNextCard(hand, i, 1) * render_target.w / 300;
      }
      if (getHoverProgressOfNextCard(hand, i, 0) > 0) {
        // Shift to left
        d_render_target.x -= getHoverProgressOfNextCard(hand, i, 0) * render_target.w / 300;
      }
      // Render the card (based on state)
      if (hand->card_hover_progress[i] == 0) {
        // Card sits in hand
      } else if (hand->card_hover_progress[i] > 0) {
        // Card moves up, rotates less
        d_render_target.y -= hand->card_hover_progress[i] * (render_target.h + HAND_MAX_TOP * hand_render_scale - target.h) / 100;
        hand->deck->cards[i]->sprite->rotation *= (float)(100 - hand->card_hover_progress[i]) / 100.0;
      } else {
        // Card is moved down in hand
        d_render_target.y -= hand->card_hover_progress[i] * target.h / 100;
        // Make this card take up less horizontal space (remember, card_hover_prog is neg!)
        render_target.x += HAND_OFFSET * hand->card_hover_progress[i] * hand_render_scale / 100;
      }

      // Shift to follow position if selected
      if (hand->card_follow_distance[i] < 100) {
        d_render_target.x = HAND_FOLLOW_OFFSET * hand_render_scale + mouse_x * (100 - hand->card_follow_distance[i]) / 100 + d_render_target.x * hand->card_follow_distance[i] / 100;
        d_render_target.y = HAND_FOLLOW_OFFSET * hand_render_scale + mouse_y * (100 - hand->card_follow_distance[i]) / 100 + d_render_target.y * hand->card_follow_distance[i] / 100;
        hand->deck->cards[i]->sprite->rotation *= (float)(hand->card_follow_distance[i]) / 100.0;
      }

      // Shift all cards down if card is selected
      d_render_target.y += HAND_MAX_TOP * 2 * (100 - getMinFollowDistance(hand)) / 100 * hand->card_follow_distance[i] / 100 * hand_render_scale;

      // Update the hovered state of this card
      if (
        d_render_target.x <= mouse_x &&
        d_render_target.y <= mouse_y &&
        d_render_target.x + d_render_target.w >= mouse_x &&
        d_render_target.y + d_render_target.h >= mouse_y &&
        hand->card_hover_progress[i] >= 0 // Appearing cards can not be hovored!
      ) {
        // Store the card that is pointed at
        card_hovered = i;
      }

      // Render the card
      renderSprite(renderer, hand->deck->cards[i]->sprite, frame, &d_render_target);

      // Increment j and move the render target to the next card
      render_target.x += HAND_OFFSET * hand_render_scale;
      if (j < card_count / 2) {
        render_target.y -= 2 * HAND_MAX_TOP * hand_render_scale / card_count;
      } else {
        render_target.y += 2 * HAND_MAX_TOP * hand_render_scale / card_count;
      }
      j ++;
    }
  }

  // Animate the cards
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    if (i == card_hovered) {
      // Animate the hovered card
      hand->card_hover_progress[card_hovered] += HAND_HOVER_ANIM_SPEED + HAND_HOVER_ANIM_SPEED * (hand->card_hover_progress[card_hovered]) / 10;
      if (hand->card_hover_progress[card_hovered] > 100) {
        hand->card_hover_progress[card_hovered] = 100;
      }
    } else if (hand->deck->card_used[i] || hand->card_remove_progress[i] > 0) {
      // Restore card to 0 hover progress
      if (hand->card_hover_progress[i] < 0) {
        hand->card_hover_progress[i] += HAND_HOVER_ANIM_SPEED + HAND_HOVER_ANIM_SPEED * (-hand->card_hover_progress[i]) / 10;
        // Make sure we always hit zero
        if (hand->card_hover_progress[i] > -HAND_HOVER_ANIM_SPEED) {
          hand->card_hover_progress[i] = 0;
        }
      } else if (hand->card_hover_progress[i] > 0) {
        hand->card_hover_progress[i] -= HAND_HOVER_ANIM_SPEED + HAND_HOVER_ANIM_SPEED * (hand->card_hover_progress[i]) / 10;
        // Make sure we always hit zero
        if (hand->card_hover_progress[i] < HAND_HOVER_ANIM_SPEED) {
          hand->card_hover_progress[i] = 0;
        }
      }
    }
    // Update the remove progress
    if (hand->card_remove_progress[i] > 0) {
      hand->card_remove_progress[i] += HAND_REMOVE_ANIM_SPEED + HAND_REMOVE_ANIM_SPEED * (101 - hand->card_remove_progress[i]) / 10;
      // Reset remove progress (and thus hide card)
      if (hand->card_remove_progress[i] > 100) {
        hand->card_remove_progress[i] = 0;
      }
    }
    // Animate the follow distance
    if (hand->card_selected == i) {
      hand->card_follow_distance[i] -= HAND_FOLLOW_SPEED + HAND_FOLLOW_SPEED * hand->card_follow_distance[i] / 10;
      if (hand->card_follow_distance[i] < 0) {
        hand->card_follow_distance[i] = 0;
      }
    } else if (hand->card_follow_distance[i] < 100) {
      hand->card_follow_distance[i] += HAND_FOLLOW_SPEED + HAND_FOLLOW_SPEED * (101 - hand->card_follow_distance[i]) / 10;
      if (hand->card_follow_distance[i] > 100) {
        hand->card_follow_distance[i] = 100;
      }
    }
  }

  // Store the hovored card
  hand->card_hovered = card_hovered;
}

void pauseHandSound(Hand* hand, int is_paused) {
  // Play / Pause audio
  SDL_PauseAudioDevice(hand->audio_device, is_paused == 0 ? 0 : 1);
}

void updateHand(Hand* hand, SDL_Event* e, void* payload) {
  // Select / Unselect / Play cards on mouse events
  if (e->type == SDL_MOUSEBUTTONUP) {
    if (((SDL_MouseButtonEvent*)e)->button == SDL_BUTTON_LEFT) {
      // Left click -> play or select
      if (hand->card_selected != -1) {
        // Count the cards in the deck NOTE: Sticky cards are disregarded
        int card_count = 0;
        for (int i = 0; i < lengthof(hand->deck->card_used); i ++) {
          if (hand->deck->card_used[i] && hand->deck->cards[i]->sticky != 1) {
            card_count ++;
          }
        }
        // Play the card NOTE: Card cost is enforced here, maybe better to do this in the game (?)
        if (card_count >= hand->deck->cards[hand->card_selected]->cost && hand->play(hand->deck->cards[hand->card_selected]->type, payload)) {
          // Play successful, remove the card from the hand (and deselect it)
          hand->deck->card_used[hand->card_selected] = 0;
          hand->card_remove_progress[hand->card_selected] = 1;
          // Discard the cost - 1
          discardCards(hand, hand->deck->cards[hand->card_selected]->cost - 1);
          // Deselect the card
          hand->card_selected = -1;
          // Play sound effect
          playSoundEffect(hand, Place);
        } else {
          // Play unsuccessful, deselect the card
          hand->card_selected = -1;
          // Play sound effect
          playSoundEffect(hand, Shove);
        }
      } else if (hand->card_hovered != -1) {
        // Select the hovered card
        hand->card_selected = hand->card_hovered;
      }
    } else {
      // Right click (deselect card, if selected)
      if (hand->card_selected != -1) {
        playSoundEffect(hand, Shove);
      }
      hand->card_selected = -1;
    }
  }
}

void fillHand(Hand* hand, int full_count) {
  // Add random cards that are not used into the hand
  int card_count = 0;
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    if (hand->deck->card_used[i] && hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1) {
      card_count ++;
    }
  }
  // Play the sound effect
  if (card_count < full_count) {
    //playSoundEffect(hand, Draw);
  }
  // Add random cards that are not in the hand
  for (int i = 0; card_count < full_count && i < 1000; i++) {
    if (
      !hand->deck->card_used[i % lengthof(hand->deck->cards)] &&
      hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1 &&
      rand() % 100 > 80
    ) {
      hand->deck->card_used[i % lengthof(hand->deck->cards)] = 1;
      hand->card_hover_progress[i % lengthof(hand->deck->cards)] = -100;
      card_count ++;
    }
  }
}

void drawCards(Hand* hand, int count) {
  // Determine number of cards in hand
  int card_count = 0;
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    if (hand->deck->card_used[i] && hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1) {
      card_count ++;
    }
  }
  // Add random cards till the hand is full or count is reached
  int added_count = 0;
  for (int i = 0; added_count + card_count < lengthof(hand->deck->cards) && added_count < count && i < 1000; i++) {
    if (
      !hand->deck->card_used[i % lengthof(hand->deck->cards)] &&
      hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1 &&
      rand() % 100 > 80
    ) {
      hand->deck->card_used[i % lengthof(hand->deck->cards)] = 1;
      hand->card_hover_progress[i % lengthof(hand->deck->cards)] = -100;
      added_count ++;
    }
  }
  // Play the sound
  if (added_count > 0) {
    //playSoundEffect(hand, Draw);
  }
}
void discardCards(Hand* hand, int count) {
  // Determine number of cards in hand
  int card_count = 0;
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    if (hand->deck->card_used[i] && hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1) {
      card_count ++;
    }
  }
  // Discard random cards till the hand is empty or count is reached
  int removed_count = 0;
  for (int i = 0; card_count > 0 && removed_count < count && i < 1000; i++) {
    if (
      hand->deck->card_used[i % lengthof(hand->deck->cards)] &&
      hand->deck->cards[i % lengthof(hand->deck->cards)]->sticky != 1 &&
      rand() % 100 > 80
    ) {
      hand->deck->card_used[i % lengthof(hand->deck->cards)] = 0;
      hand->card_remove_progress[i % lengthof(hand->deck->cards)] = 1;
      removed_count ++;
      card_count --;
    }
  }
}

void drawCard(Hand* hand, int id) {
  // Add the card by it's id, unless it does not exist in the deck
  if (id >= 0 && id < lengthof(hand->deck->cards)) {
    hand->deck->card_used[id] = 1;
    hand->card_hover_progress[id] = -100;
  }
}

void reshuffleHand(Hand* hand, int full_count) {
  // Remove all the cards from the hands and create a new random cards
  for (int i = 0; i < lengthof(hand->deck->cards); i ++) {
    hand->deck->card_used[i] = 0;
  }
  fillHand(hand, full_count);
}
