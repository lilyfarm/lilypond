#include "level.h"

#include "player.h"
#include "state.h"
#include "test.h"
#include <errno.h>

static const struct token_entry TOKENS[] = {
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {' ', SPRITE_NONE, NULL}};

static const size_t TOKEN_SIZE = 4;

// Based on our current settings of the screen size being 320 x 240, and the
// sprite size being 16, each level should be 20 x 15 characters long. If we
// decide to modify these settings in the future, we will probably need to
// modify these tests as well.

static void test_invalid_levels(void) {
  const char *too_short = "     "
                          "     ";

  struct level *l = level_load_from_string(too_short, 1, 1, TOKENS, TOKEN_SIZE);
  assert(l == NULL);
  assert(errno == EINVAL);

  errno = 0; // reset errno

  const char *too_long = "                     " // 21 characters
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     "
                         "                     ";
  l = level_load_from_string(too_long, 1, 1, TOKENS, TOKEN_SIZE);
  assert(l == NULL);
  assert(errno == EINVAL);
}

static void test_empty_level(void) {
  char empty[] = "                    " // 20 characters
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "
                 "                    "; // 15 lines

  // reset errno
  errno = 0;

  struct level *l = level_load_from_string(empty, 1, 1, TOKENS, TOKEN_SIZE);

  // this should be NULL as the world has no player
  assert(l == NULL);
  assert(errno == EINVAL);

  // Now let's include a player (P) into the level
  empty[0] = 'P';

  errno = 0; // reset errno
  l = level_load_from_string(empty, 1, 1, TOKENS, TOKEN_SIZE);

  LOG_ERROR("%d", errno);

  assert(l != NULL);
  assert(errno == 0);

  l->active_sprites->free_on_clean = false;
  l->active_sprites->destroy_on_clean = false;

  level_free(&l);
  g_game.player->s = NULL;
}

static void test_wall_level(void) {
  char wall[] = "                    " // 20 characters
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "P                   "
                "===================="
                "********************"; // 15 lines

  // reset errno
  errno = 0;

  struct level *l = level_load_from_string(wall, 1, 1, TOKENS, TOKEN_SIZE);

  assert(l != NULL);
  assert(errno == 0);

  register size_t c;
  // on the 14th row, each column should be SPRITE_WALL_TOP
  // on the 15th row, each column should be SPRITE_WALL
  for (c = 0; c < 20; c++) {
    assert(l->passive_sprites[13 * 20 + c]->id == SPRITE_WALL_TOP);
    assert(l->passive_sprites[14 * 20 + c]->id == SPRITE_WALL);
  }

  l->active_sprites->free_on_clean = false;
  l->active_sprites->destroy_on_clean = false;

  level_free(&l);
  g_game.player->s = NULL;
}

// level larger than the screen
static void test_wide_level(void) {
  char wall[] = "                    " // 20 characters
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "P                   "
                "===================="
                "********************"; // 15 lines

  // reset errno
  errno = 0;

  // otherwise completely valid level, but not the right width (2 *
  // COLUMN_COUNT)
  struct level *l = level_load_from_string(wall, 2, 1, TOKENS, TOKEN_SIZE);

  assert(l == NULL);
  assert(errno == EINVAL);

  char wide_wall[] = "                                        " // 40 characters
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "                                        "
                     "P                                       "
                     "========================================"
                     "****************************************"; // 15 lines

  l = level_load_from_string(wide_wall, 2, 1, TOKENS, TOKEN_SIZE);

  register size_t c;
  // on the 14th row, each column should be SPRITE_WALL_TOP
  // on the 15th row, each column should be SPRITE_WALL
  for (c = 0; c < 40; c++) {
    assert(l->passive_sprites[13 * 40 + c]->id == SPRITE_WALL_TOP);
    assert(l->passive_sprites[14 * 40 + c]->id == SPRITE_WALL);
  }

  l->active_sprites->free_on_clean = false;
  l->active_sprites->destroy_on_clean = false;

  level_free(&l);
  g_game.player->s = NULL;
}

static void test_high_level(void) {
  char wall[] = "                    " // 20 characters
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "P                   "
                "===================="
                "********************"
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "===================="
                "********************"; // 15 * 2 lines

  // reset errno
  errno = 0;

  struct level *l = level_load_from_string(wall, 1, 2, TOKENS, TOKEN_SIZE);

  assert(l != NULL);
  assert(errno == 0);

  register size_t c;
  // on the 14th and 29th row, each column should be SPRITE_WALL_TOP
  // on the 15th and 30th row, each column should be SPRITE_WALL
  for (c = 0; c < 20; c++) {
    assert(l->passive_sprites[13 * 20 + c]->id == SPRITE_WALL_TOP);
    assert(l->passive_sprites[28 * 20 + c]->id == SPRITE_WALL_TOP);
    assert(l->passive_sprites[14 * 20 + c]->id == SPRITE_WALL);
    assert(l->passive_sprites[29 * 20 + c]->id == SPRITE_WALL);
  }

  l->active_sprites->free_on_clean = false;
  l->active_sprites->destroy_on_clean = false;

  level_free(&l);
  g_game.player->s = NULL;
}

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  // load all sprite types prior to testing, otherwise the assert_not_nulls will
  // fail
  assert(g_sprite_types_create() == 0);

  struct player *p = malloc(sizeof(struct player));
  assert(p != NULL);
  g_game.player = p; // we don't want to fail because the player is NULL
  assert(player_create(p) == 0);

  RUN_TEST(test_invalid_levels);
  RUN_TEST(test_empty_level);
  RUN_TEST(test_wall_level);
  RUN_TEST(test_wide_level);
  RUN_TEST(test_high_level);

  player_destroy(&p);
  g_sprite_types_destroy();

  return EXIT_SUCCESS;
}
