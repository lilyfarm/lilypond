#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// sound_channel contains all the channels used in the game
enum sound_channel {
  // channel for sounds generated by the player
  CHANNEL_PLAYER = 0,
  // channel for sound effects created by other sprites
  CHANNEL_SPRITE,
  // channel for message and user interface related sounds
  CHANNEL_UI,
  // channel for sounds created by an enemy without player collision
  CHANNEL_ENEMY,
  // CHANNEL_COUNT should always be the last element in this enum as it is the
  // number of channels we initialize SDL2_Mixer with
  CHANNEL_COUNT
};

// sound_id contains the types of sound effects we have in the game,
enum sound_id {
  // when a coin is collected
  SOUND_COIN = 0,
  // when an extra life is collected
  SOUND_EXTRA_LIFE,
  // when the player is killed
  SOUND_KILL,
  // when the player hits an enemy
  SOUND_HIT,
  // when a message is opened
  SOUND_MESSAGE,
  // when a door is opened
  SOUND_DOOR,
  // when the player jumps
  SOUND_JUMP,
  // when the player does a short jump
  SOUND_SHORT_JUMP,
  // when the menu moves
  SOUND_MENU_MOVE,
  // when a menu option is select
  SOUND_MENU_SELECT,
  // when the player jumps on a spring
  SOUND_SPRING,
  // SOUND_COUNT should always be the last element in this enum as it is the
  // size of the g_sounds array
  SOUND_COUNT
};

// sound_create sets up the state necessary to have sound in the game. Returns 0
// on success, -1 on failure.
int sound_create(void);

// sound_destroy destroys all sound state and frees up used resources
void sound_destroy(void);

// sound_play plays the sound with id `id` at channel `c`. Returns 0 on success,
// -1 on failure.
int sound_play(const enum sound_id id, const enum sound_channel c);

#endif // SOUND_H
