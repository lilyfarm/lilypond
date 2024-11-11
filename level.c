#include "level.h"

#include "player.h"
#include "render.h"
#include "safe.h"
#include "state.h"
#include "util.h"
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

int level_iterate(struct level *l) {
  assert_not_null(2, l, g_game.player);

  register size_t i;
  struct array *s_arr = l->active_sprites;
  for (i = 0; i < s_arr->l; i++) {
    struct sprite *s = s_arr->a[i];
    struct sprite_type *t = s->type;

    // skip the player as well as active sprites that are removed
    if (t->id == SPRITE_PLAYER || s->removed) {
      continue;
    }

    if (t->frame_handler(s) != 0) {
      LOG_ERROR("failed to handle [frame handler] sprite with id: %d",
                s->type->id);
      return -1;
    }

    // if the active sprite collides with the player, do whatever the active
    // sprite is supposed to do upon collision
    if (util_collide(s, g_game.player->s)) {
      if (t->hit_handler(s) != 0) {
        LOG_ERROR("failed to handle [hit handler] sprite with id: %d",
                  s->type->id);
        return -1;
      }
    }
  }

  // clean the sprites in the level that were removed
  array_clean(s_arr);

  return 0;
}

// level_new initializes a new level or returns NULL on failure. The user is
// responsible for deallocating the level using level_free. They are to be
// handled externally by the functions of the world this level is in.
static struct level *level_new(const size_t w, const size_t h) {
  struct level *l = (struct level *)malloc(sizeof(struct level));
  if (l == NULL) {
    // errno = ENOMEM
    goto error_out;
  }

  l->player_found = false;

  l->active_sprites = array_new();
  if (l->active_sprites == NULL) {
    goto error_post_l;
  }

  l->w = w;
  l->h = h;

  size_t passive_arr_len;
  if (SDL_size_mul_overflow(SPRITE_COUNT, w, &passive_arr_len) != 0) {
    errno = EOVERFLOW;
    goto error_post_active_sprites;
  }

  if (SDL_size_mul_overflow(passive_arr_len, h, &passive_arr_len) != 0) {
    errno = EOVERFLOW;
    goto error_post_active_sprites;
  }

  size_t passive_arr_size;
  if (SDL_size_mul_overflow(passive_arr_len, sizeof(struct sprite_type *),
                            &passive_arr_size) != 0) {
    errno = EOVERFLOW;
    goto error_post_active_sprites;
  }

  l->passive_sprites = malloc(passive_arr_size);
  if (l->passive_sprites == NULL) {
    // errno = ERRNOMEM
    goto error_post_active_sprites;
  }

  // l has been successfully allocated if we reached this point.
  register size_t r;
  register size_t c;

  // set all the passive sprites to SPRITE_NONE to start with
  for (r = 0; r < ROW_COUNT * h; r++) {
    for (c = 0; c < COLUMN_COUNT * w; c++) {
      l->passive_sprites[r * COLUMN_COUNT * w + c] =
          g_sprite_types[SPRITE_NONE];
    }
  }

  return l;

error_post_active_sprites:
  array_free(&l->active_sprites);
error_post_l:
  free(l);
error_out:
  return NULL;
}

void level_free(struct level **pl) {
  struct player *p = g_game.player;
  assert_not_null(3, p, pl, *pl);

  struct level *l = *pl;
  array_free(&l->active_sprites);
  free(l);
  *pl = NULL;

  LOG_INFO_VERBOSE("unloaded level");
}

struct level *level_load_from_string(const char *s, const size_t w,
                                     const size_t h,
                                     const struct token_entry *arr,
                                     const size_t arr_len) {
  struct level *l = level_new(w, h);
  if (l == NULL) {
    return NULL;
  }

  if (token_level_populate(l, s, arr, arr_len) != 0) {
    level_free(&l);
    return NULL;
  }

  return l;
}

int level_load(const char *filename, const struct token_entry *arr,
               const size_t arr_len) {
  struct level *prev = NULL;
  if (g_game.level != NULL) {
    prev = g_game.level;
  }

  char *str;
  size_t w, h;
  if (util_level_from_file(filename, &str, &w, &h) < 0) {
    // logging is done in util_level_from_file
    return -1;
  }

  // so the new level can add its sprite here. The pointer to this old sprite is
  // still in the active sprites array of the previous level and hence will be
  // freed when we cann level_free on the previous level
  g_game.player->s = NULL;
  // Side note: we are hoping here the system has enough memory to load two
  // levels at once. Otherwise, we may have to optimize by free-ing the previous
  // world first.
  struct level *l = level_load_from_string(str, w, h, arr, arr_len);

  if (l == NULL) {
    return -1;
  }

  if (prev != NULL) {
    level_free(&prev);
  }

  g_game.level = l;

  return 0;
}
