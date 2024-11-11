#include "handlers.h"

#include "fps.h"
#include "level.h"
#include "player.h"
#include "safe.h"
#include "sound.h"
#include "state.h"
#include "token.h"
#include "util.h"

static const Sint64 SHOOT_DELAY = 1000;
static const int SHOT_FPS = 9;

// SHOT_SPEED is the horizontal velocity of the shot
static const Sint64 SHOT_SPEED = 72;

int handler_shot_init(struct sprite *s) {
  assert_not_null(1, s);

  sprite_animation_set_frame(s, 1, 2, SHOT_FPS);
  return 0;
}

int handler_shot_frame(struct sprite *s) {
  assert_not_null(1, s);

  if (s->removed) {
    return 0;
  }

  // process horizontal movement
  enum collision coll = util_move_x(s);

  if (coll != COLLISION_NONE) {
    s->removed = true;
  }

  return 0;
}

int handler_shot_hit(struct sprite *s) {
  struct player *p = g_game.player;
  assert_not_null(3, s, p, p->s);

  s->removed = true;
  player_kill(p);

  return 0;
}

int handler_ghost_init(struct sprite *s) {
  assert_not_null(1, s);

  if (handler_spider_init(s) != 0) {
    return -1;
  }

  struct fps_timer *t = fps_timer_create(SHOOT_DELAY);
  if (t == NULL) {
    return -1;
  }

  s->data.enemy.shoot_timer = t;
  return 0;
}

int handler_ghost_frame(struct sprite *s) {
  struct player *p = g_game.player;
  struct level *l = g_game.level;
  assert_not_null(4, l, p, s, p->s);

  if (handler_spider_frame(s) != 0) {
    return -1;
  }

  if (!s->data.enemy.alive) {
    fps_timer_destroy(&s->data.enemy.shoot_timer);
    return 0;
  }

  struct fps_timer *shoot_timer = s->data.enemy.shoot_timer;
  fps_timer_iterate(shoot_timer);

  // don't shoot if the delay has not been reached
  if (!fps_timer_done(shoot_timer)) {
    return 0;
  }

  if (util_visible(s, p->s)) {
    fps_timer_reset(shoot_timer);

    int r, c;
    util_nearest(s, &r, &c);

    if (token_active_sprite(l, SPRITE_SHOT, r, c) != 0) {
      return -1;
    }

    // the shot is the most recently created sprite
    struct sprite *s_shot = l->active_sprites->a[l->active_sprites->l - 1];
    assert(s_shot != NULL);

    s_shot->vx = s->vx > 0 ? SHOT_SPEED : -SHOT_SPEED;
    s_shot->animation.flip = s->vx > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
  }

  return 0;
}
