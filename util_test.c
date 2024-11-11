#include "util.h"

#include "fps.h"
#include "level.h"
#include "player.h"
#include "state.h"
#include "test.h"
#include <errno.h>

#define TOKEN_SIZE 6

static const struct token_entry TOKENS[TOKEN_SIZE] = {
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {' ', SPRITE_NONE, NULL}};

static struct level *l = NULL;
static struct player p;

// this test expects a 15 row, 20 column level. May need to be changed if the
// level size changes.
static void test_util_nearest(void) {
  int r, c;
  struct sprite s;
  struct sprite_type t;
  // so we have a sprite type body
  s.type = &t;

  s.x = 0;
  s.y = 0;
  t.body = (SDL_Rect){0, 0, 16, 16};

  util_nearest(&s, &r, &c);
  assert(r == 0);
  assert(c == 0);

  s.x = LEVEL_WIDTH;
  util_nearest(&s, &r, &c);
  assert(r == 0);
  assert(c == 20);

  s.x = 0;
  s.y = LEVEL_HEIGHT;
  util_nearest(&s, &r, &c);
  assert(r == 15);
  assert(c == 0);

  s.x = LEVEL_WIDTH / 2;
  s.y = LEVEL_HEIGHT / 2;
  util_nearest(&s, &r, &c);

  assert(c == 10);
  assert(r == 8);

  t.body = (SDL_Rect){0, 0, 15, 15};
  util_nearest(&s, &r, &c);

  assert(r == 7);
  assert(c == 10);

  s.x += SPRITE_SIZE / 2;
  util_nearest(&s, &r, &c);
  assert(r == 7);
  assert(c == 10);

  t.body = (SDL_Rect){0, 0, 16, 16};
  util_nearest(&s, &r, &c);
  assert(r == 8);
  assert(c == 11);
}

static void test_util_other_sprites(void) {
  assert(util_door(10, 18));
  assert(!util_door(10, 17));
  assert(!util_door(0, 0));

  assert(util_ladder(0, 0));
  assert(!util_ladder(10, 10));

  // these should be false and *not* a segmentation fault as these are out of
  // the level
  assert(!util_door(300, 0));
  assert(!util_door(0, 300));
  assert(!util_ladder(300, 0));
  assert(!util_ladder(0, 300));
}

static void test_util_solid(void) {
  assert(!util_solid(0, 0, SOLID_TOP));
  assert(util_solid(13, 0, SOLID_TOP));
  assert(!util_solid(14, 0, SOLID_TOP));
  assert(util_solid(14, 0, SOLID_LEFT));

  // horizontally out of the screen is considered solid
  assert(util_solid(0, -1, SOLID_RIGHT));
  assert(util_solid(0, 20, SOLID_LEFT));

  // vertically out of the screen is *not* considered solid
  assert(!util_solid(-1, 0, SOLID_BOTTOM));
  assert(!util_solid(15, 0, SOLID_TOP));
}

static void test_util_move_x(void) {
  // we need some frame time to test this
  fps_init();
  SDL_Delay(50); // 50 ms should do the trick
  fps_iterate();

  p.s->vx = -72;
  assert(util_move_x(p.s) == COLLISION_LEFT);
  p.s->vx = 72;
  assert(util_move_x(p.s) == COLLISION_NONE);

  p.s->vx = 16 - p.s->type->body.w + 1;
  p.s->x = 7 * SPRITE_SIZE;
  assert(util_move_x(p.s) == COLLISION_RIGHT);
}

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  // load all sprite types prior to testing, otherwise the assert_not_nulls will
  // fail
  assert(g_sprite_types_create() == 0);
  g_game.player = &p; // we don't want to fail because the player is NULL
  p.s = NULL;

  char test_level[] = "L                   " // 20 characters
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                    "
                      "                  D "
                      "                    "
                      "P       *    =      "
                      "========*====*======"
                      "********************"; // 15 lines

  // reset errno
  errno = 0;

  l = level_load_from_string(test_level, 1, 1, TOKENS, TOKEN_SIZE);

  assert(l != NULL);
  assert(errno == 0);

  g_game.level = l;

  RUN_TEST(test_util_nearest);
  RUN_TEST(test_util_other_sprites);
  RUN_TEST(test_util_solid);
  RUN_TEST(test_util_move_x);

  level_free(&l);
  g_sprite_types_destroy();

  return EXIT_SUCCESS;
}
