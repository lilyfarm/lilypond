#include "token.h"
#include "level.h"
#include "player.h"
#include "safe.h"
#include "state.h"
#include <errno.h>

// technically 128 but most computers use 8 bits for a char
#define LEN_ASCII 256

int token_passive_sprite(struct level *l, const enum sprite_id id,
                         const size_t r, const size_t c) {
  // some quick validation
  assert(r < ROW_COUNT * l->h && c < COLUMN_COUNT * l->w);
  assert_not_null(1, l);

  l->passive_sprites[r * COLUMN_COUNT * l->w + c] = g_sprite_types[id];
  return 0;
}

int token_active_sprite(struct level *l, const enum sprite_id id,
                        const size_t r, const size_t c) {
  assert_not_null(1, l);

  struct sprite *s = (struct sprite *)malloc(sizeof(struct sprite));
  if (s == NULL) {
    // errno = ENOMEM
    LOG_ERROR("failed to allocate new active sprite");
    return -1;
  }

  if (sprite_init(s, id) != 0) {
    goto error_out;
  }

  s->x = SPRITE_SIZE * c;
  s->y = SPRITE_SIZE * r;

  if (array_append(l->active_sprites, s) != 0) {
    LOG_ERROR("failed to append new active sprite to level");
    goto error_out;
  }

  return 0;

error_out:
  free(s);
  return -1;
}

int token_player(struct level *l, const enum sprite_id id, const size_t r,
                 const size_t c) {
  LOG_ERROR("TOKEN PLAYER");
  SAFE_UNUSED(id);
  if (l->player_found) { // something went wrong here, there can only
                         // be only player in a level
    LOG_ERROR("player already added, more than 1 player not allowed per level");
    return -1;
  }

  // set the player's start position
  struct player *p = g_game.player;

  assert_not_null(1, p);
  // player->sprite should have been freed by the previous level or set to NULL
  // if it is the first level
  assert(p->s == NULL);

  struct sprite *s = (struct sprite *)malloc(sizeof(struct sprite));
  if (s == NULL) {
    LOG_ERROR("could not allocate player->sprite");
    return -1;
  }

  p->s = s;

  // initialize the player's sprite
  if (sprite_init(p->s, SPRITE_PLAYER) != 0) {
    return -1;
  }

  p->s->x = SPRITE_SIZE * c;
  p->s->y = SPRITE_SIZE * r;

  player_respawn_update(p);

  // add player to the level
  if (array_append(l->active_sprites, p->s) != 0) {
    // errno = ENOMEM
    return -1;
  }

  l->player_found = true;
  return 0;
}

int token_level_populate(struct level *l, const char *s,
                         const struct token_entry *arr, const size_t arr_len) {
  assert_not_null(3, l, s, arr);

  // Some multiplication overflow checks
  size_t overflow_check;

  if (SDL_size_mul_overflow(SPRITE_COUNT, l->w, &overflow_check) != 0) {
    goto overflow_error;
  }

  if (SDL_size_mul_overflow(overflow_check, l->h, &overflow_check) != 0) {
    goto overflow_error;
  }

  // we need to make sure the level is EXACTLY the expected length. Since the
  // strlen family of functions return the number of characters that PRECEDE the
  // null terminator i.e NOT including the null terminator, we do not need to
  // add an extra 1 here.
  size_t len_expected = overflow_check; // rename to be clearer
  size_t len_check = strnlen(s, len_expected);

  // s is shorter than the expected length
  if (len_check != len_expected) {
    LOG_ERROR("size of level string is too small. Expected size: %lu",
              (unsigned long)len_expected);
    goto invalid_error;
  }

  // len_check == len_expected
  // however, this could also mean s is *longer* than the expected length. To
  // ensure that is not the case, we make sure the last character is a null
  // terminator.
  if (s[len_check] != '\0') {
    LOG_ERROR("size of level string is too large. Expected size: %lu",
              (unsigned long)len_expected);
    goto invalid_error;
  }

  // iterate over each character in the level and make the sprite
  // corresponding to the character.
  register size_t r;
  register size_t c;

  // the total number of columns in the level
  size_t level_cols;
  if (SDL_size_mul_overflow(COLUMN_COUNT, l->w, &level_cols) != 0) {
    goto overflow_error;
  }

  // the total number of rows in the level
  size_t level_rows;
  if (SDL_size_mul_overflow(ROW_COUNT, l->h, &level_rows) != 0) {
    goto overflow_error;
  }

  const struct token_entry *token_map[LEN_ASCII] = {NULL};
  register size_t i;

  for (i = 0; i < arr_len; i++) {
    token_map[(size_t)arr[i].t] = &arr[i];
  }

  for (r = 0; r < level_rows; r++) {   // iterate over the rows
    for (c = 0; c < level_cols; c++) { // iterate over the columns
      const char token = s[r * level_cols + c];
      const struct token_entry *entry = token_map[(size_t)token];

      // we could not recognize the token
      if (entry == NULL) {
        LOG_ERROR("failed to handle token: %c", token);
        goto invalid_error;
      }

      // it is NULL in the case of SPRITE_NONE, which is allowed
      if (entry->f != NULL) {
        if (entry->f(l, entry->s, r, c) != 0) {
          return -1;
        }
      }
    }
  }

  // every level needs to specify a player
  if (!l->player_found) {
    LOG_ERROR("player not found");
    goto invalid_error;
  }

  // After every sprite has been added, sort the level's active sprites by
  // their depth
  array_sort(l->active_sprites);
  LOG_INFO_VERBOSE("loaded level from level file");
  return 0;

overflow_error:
  LOG_ERROR("size_t overflow");
  errno = EOVERFLOW;
  return -1;

invalid_error:
  errno = EINVAL;
  return -1;
}
