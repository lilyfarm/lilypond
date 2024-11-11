#include "handlers.h"

#include "fps.h"
#include "level.h"
#include "player.h"
#include "safe.h"
#include "sound.h"
#include "state.h"
#include "token.h"
#include "util.h"

// WALK is the horizontal velocity while the skeleton is walking, in logical
// pixels per second
static const double WALK = 36;
// SPRINT is the horizontal velocity while the skeleton is sprinting, in logical
// pixels per second
static const double SPRINT = 142;

int handler_skeleton_init(struct sprite *s) {
  assert_not_null(1, s);

  if (handler_spider_init(s) != 0) {
    return -1;
  }

  return 0;
}

int handler_skeleton_frame(struct sprite *s) {
  struct player *p = g_game.player;
  struct level *l = g_game.level;
  assert_not_null(4, l, p, s, p->s);

  if (util_visible(s, p->s)) {
    s->vx = s->vx < 0 ? -SPRINT : SPRINT;
  } else {
    s->vx = s->vx < 0 ? -WALK : WALK;
  }

  if (handler_spider_frame(s) != 0) {
    return -1;
  }

  if (!s->data.enemy.alive) {
    return 0;
  }

  return 0;
}
