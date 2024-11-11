#ifndef LEVEL_H
#define LEVEL_H

#include "array.h"
#include "base.h"
#include "token.h"

#include <stdbool.h>

//  --------------------------------------------
// | Quick note on the structure of the game    |
// | A SPRITE is our atomic unit                |
// | A LEVEL is a set of SPRITEs                |
// | A GAME is a set of LEVELs                  |
//  --------------------------------------------

// logic for displaying levels in the game. A level has two kinds of sprites:
// passive sprites and active sprites.
// An active sprite:
// - is rendered
// - may be animated
// - is tested for collisions
// - may move
// - is rendered *after* any passive sprite
// - has depth
// A passive sprite:
// - is rendered
// - is rendered *before* any active sprite
struct level {
  // passive_sprites stores (pointers to) all the passive sprites in the level.
  // This is a dynamic array of size ROW_COUNT * COLUMN_COUNT * w (or, more
  // simply, SPRITE_COUNT * w).
  struct sprite_type **passive_sprites;
  // active_sprites stores all the active sprites in the level
  struct array *active_sprites;
  // w is the width of the level in units of COLUMN_COUNT i.e "screen". How many
  // screens wide is the level?
  size_t w;
  // h is the height of the level in units of ROW_COUNT i.e "screen". How many
  // screens high is the level?
  size_t h;
  // player_found indicates whether or not we identified where the player is in
  // the level
  bool player_found;
};

// level_load_from_string loads a level directly from its string, given a level
// width. Primarily used for testing, or internally by level_load. Returns the
// level, or NULL on failure.
struct level *level_load_from_string(const char *s, const size_t w,
                                     const size_t h,
                                     const struct token_entry *arr,
                                     const size_t arr_len);

// level_load destroys the current world and loads a new one, the string
// representation for which is in the file `filename`. Returns 0 on success, -1
// on failure. If this function is successful, current_level will be set to the
// loaded level.
int level_load(const char *filename, const struct token_entry *arr,
               const size_t arr_len);

// level_free deallocates the memory used for a level and sets the value
// pointed to by pl to NULL
void level_free(struct level **pl);

// level_iterate processes all active sprites and other objects that require
// processing per frame within the lever *except* for the player, which is
// handled separately. Returns 0 on success, -1 on failure.
int level_iterate(struct level *l);

#endif // LEVEL_H
