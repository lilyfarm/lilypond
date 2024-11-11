#include "handlers.h"

#include "fps.h"
#include "player.h"
#include "safe.h"
#include "sound.h"
#include "state.h"
#include "util.h"

// WALK is the horizontal velocity while the spider is walking, in logical
// pixels per second
static const Sint64 WALK = 36;

// BOUNCE is the vertical velocity the player gets after jumping on a spider.
static const Sint64 BOUNCE = 128;

// For animations:
static const Sint64 ANIM_WALK = 8; // frames per second
static const Sint64 ANIM_FLY = 3;  // frames per second

// REMOVE_DELAY is the time the spider remains on the screen for after being
// squished by the player.
static const Sint64 REMOVE_DELAY = 2000; // milliseconds

static void left(struct sprite *s) {
  s->data.enemy.dir = DIR_LEFT;
  s->animation.flip = SDL_FLIP_HORIZONTAL;
  s->vx = -WALK;
}

static void right(struct sprite *s) {
  s->data.enemy.dir = DIR_RIGHT;
  s->animation.flip = SDL_FLIP_NONE;
  s->vx = WALK;
}

int handler_spider_init(struct sprite *s) {
  assert_not_null(1, s);

  if (util_fair_coin_flip()) {
    left(s);
  } else {
    right(s);
  }

  s->data.enemy.alive = true;
  struct fps_timer *t = fps_timer_create(REMOVE_DELAY);
  if (t == NULL) {
    return -1;
  }

  s->data.enemy.remove_timer = t;

  sprite_animation_set_frame(s, 1, 2, ANIM_WALK);
  return 0;
}

int handler_spider_frame(struct sprite *s) {
  assert_not_null(1, s);

  if (!s->data.enemy.alive) {
    struct fps_timer *t = s->data.enemy.remove_timer;

    fps_timer_iterate(t);

    if (fps_timer_done(t)) {
      s->removed = true;
      fps_timer_destroy(&s->data.enemy.remove_timer);
      t = NULL;
    }

    return 0;
  }

  // process horizontal movement
  enum collision c = util_move_x(s);

  if (c & COLLISION_LEFT) {
    right(s);
  }

  if (c & COLLISION_RIGHT) {
    left(s);
  }

  return 0;
}

int handler_spider_hit(struct sprite *s) {
  struct player *p = g_game.player;
  assert_not_null(3, p, p->s, s);

  bool *alive = &s->data.enemy.alive;

  if (!*alive) {
    return 0;
  }

  if (p->air && p->s->y < s->y && p->s->vy > 0) {
    // play sound effect
    sound_play(SOUND_HIT, CHANNEL_SPRITE);

    p->s->vy = -BOUNCE;
    *alive = false;
    sprite_animation_set_frame(s, 5, 5, 0);
    return 0;
  }

  player_kill(p);
  return 0;
}

int handler_sprinting_spider_frame(struct sprite *s) {
  if (handler_spider_frame(s) != 0) {
    return -1;
  }

  if (rand() % 100 == 0) {
    s->vx *= 2;
  }

  return 0;
}

int handler_bat_init(struct sprite *s) {
  assert_not_null(1, s);

  if (handler_spider_init(s) != 0) {
    return -1;
  }

  sprite_animation_set_frame(s, 0, 1, ANIM_FLY);
  return 0;
}
