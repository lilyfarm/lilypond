#ifndef TOKEN_H
#define TOKEN_H

#include "base.h"

#include "sprite_type.h"

// forward declaration for struct level, to definition token_entry->f
struct level;

// implements functionality for handling character token to sprite creation
// mapping for levels

// contains information on what sprite to map each ASCII character to, as well
// as how to create the sprite.
struct token_entry {
  char t;           // the token
  enum sprite_id s; // the sprite id to create with the token
  // the function to use for sprite creation
  int (*f)(struct level *, const enum sprite_id, const size_t, const size_t);
};

// token_level_populate takes in a string representing a level and populates the
// passive sprites and active sprites in the level based on the string. It also
// takes in an array of token entries taken contain information on how to create
// a sprite from each ASCII character. Returns 0 on success, -1 on failure.
int token_level_populate(struct level *l, const char *s,
                         const struct token_entry *arr, const size_t arr_len);

// token_passive_sprite constructs a passive sprite. Return 0 on success, -1 on
// failure. The current implementation always returns 0 unless an assertion
// fails (in which case execution stops).
int token_passive_sprite(struct level *l, const enum sprite_id id,
                         const size_t r, const size_t c);

// token_active_sprite constructs an active sprite and adds it to the level.
// Returns 0 on success, -1 on failure.
int token_active_sprite(struct level *l, const enum sprite_id id,
                        const size_t r, const size_t c);

// token_player puts the player in the level. Returns 0 on success, -1 on error.
int token_player(struct level *l, const enum sprite_id id, const size_t r,
                 const size_t c);

#endif // TOKEN_H
