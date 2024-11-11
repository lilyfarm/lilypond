#ifndef UTIL_H
#define UTIL_H

#include "sprite_type.h"
#include <stdbool.h>

// This file contains miscellaneous utility functions

struct borders {
  // left, right, top, bottom
  double l, r, t, b;
};

// util_nearest takes a sprite and populates the nearest row and column for
// in the level for that sprite.
void util_nearest(const struct sprite *s, int *r, int *c);

// util_ladder checks if the passive sprite at row r, and column c, in the
// current level is of type SPRITE_LADDER
bool util_ladder(const int r, const int c);

// util_door checks if the passive sprite at row r, and column c, in the current
// level is of type SPRITE_DOOR
bool util_door(const int r, const int c);

// util_solid_ladder checks if the passive sprite at row r, and column c, in the
// current level is of type SPRITE_LADDER, and also if it makes 2D platformer
// physics sense for the player to be able to stay on the ladder.
bool util_solid_ladder(const int r, const int c);

// util_sprite_hints get information on the sprite's row and column on the
// level, the borders of the passive sprite in the level the sprite is nearest
// to, as well as the borders of the actual body of the sprite.
void util_sprite_hints(const struct sprite *s, int *r, int *c,
                       struct borders *passive, struct borders *actual);

// util_abs_limit maps all values v in the range (-infty, -m) to -m,
// all values v in the range [-m, m] to v, and all values v in the range
// (m, infty) to m.
double util_abs_limit(const double v, const double m);

// util_solid returns true is the passive sprite at row r and column c is
// valid (see the valid function in util.c), and if it is of the solid type
// specified.
bool util_solid(const int r, const int c, const enum solid_type s);

// util_collide returns true is sprite s1 is colliding with sprite s2. It
// returns false otherwise.
bool util_collide(const struct sprite *s1, const struct sprite *s2);

// util_return returns true if s1 can see s2
bool util_visible(const struct sprite *s1, const struct sprite *s2);

// util_level_from_file reads a level from a file. Returns length on success, -1
// on failure. User is responsible for freeing returned string. w and h store
// the width and height of the level which is inferred dynamically from the
// file.
long util_level_from_file(const char *filename, char **str, size_t *w,
                          size_t *h);

// util_fair_coin_flip returns true if 0.5 probability and false with
// 0.5 probability
bool util_fair_coin_flip(void);

// collision is returned by util operations that process movement, such as
// util_move_x
enum collision {
  COLLISION_NONE = 0,
  COLLISION_LEFT = (1 << 0),
  COLLISION_RIGHT = (1 << 1)
};

// util_move_x is a generic processor the horizontal movement of a sprite that
// also handles horizontal collisions. This is used, for example, by enemy
// handlers
enum collision util_move_x(struct sprite *s);

#endif // UTIL_H
