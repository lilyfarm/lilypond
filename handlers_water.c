#include "handlers.h"

#include "player.h"
#include "safe.h"
#include "state.h"

// For animations:
static const Sint64 ANIM_WATER = 4; // frames per second

int handler_water_init(struct sprite *s) {
  assert_not_null(1, s);

  sprite_animation_set_frame(s, 0, 3, ANIM_WATER);
  return 0;
}

int handler_water_hit(struct sprite *s) {
  SAFE_UNUSED(s);

  struct player *p = g_game.player;
  assert_not_null(2, p, p->s);

  // override blinking
  p->blink_timer->time_left = 0;
  player_kill(p);
  return 0;
}
