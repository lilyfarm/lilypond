#ifndef PLAYER_H
#define PLAYER_H

#include "base.h"

#include "fps.h"
#include "sprite.h"
#include <stdbool.h>

// ------------------------------------------------------------------
// - All functionality relating to player movement and player state -
// ------------------------------------------------------------------

// player is an extension of sprite with fields specific to a user-controlled
// player.
struct player {
  struct sprite *s; // the sprite underlying the player

  bool air;    // true if the player is mid-air
  bool ladder; // true if the player is climbing a ladder
  bool sprint; // true if the player is sprinting

  // blinking is the state that happens right after a player gets killed. The
  // player temporarily (for the duration specified by this variable, in
  // milliseconds) cannot get killed again. This timer handles the delay for
  // blinking.
  struct fps_timer *blink_timer;

  // Making these unsigned integers for easy conversion to strings, as we will
  // be probably displaying this information to the user in some way.
  unsigned int lives; // How many lives the player has.
  unsigned int coins; // The number of coins the player has collected.

  // the coordinates at which the player will be placed if killed.
  size_t respawn_x;
  size_t respawn_y;

  // the player can only jump if jump is true
  bool jump;

  // the character index of the player
  size_t index;
  // this timer handles delay between toggling characters, in milliseconds
  struct fps_timer *character_timer;
};

// player_create initializes the player. Returns 0 on success, -1 on failure.
int player_create(struct player *p);

// player_respawn_update updates the respawn position of the player to its
// current position
void player_respawn_update(struct player *p);

// player_kill kills the player unless player.blinking is non-zero
void player_kill(struct player *p);

// player_iterate processes the player state at each frame
void player_iterate(struct player *p);

// player_destroy frees all memory associated with the player and sets *pp to
// NULL
void player_destroy(struct player **pp);

#endif // PLAYER_H
