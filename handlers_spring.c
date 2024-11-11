#include "handlers.h"

#include "fps.h"
#include "player.h"
#include "safe.h"
#include "sound.h"
#include "state.h"

static Uint64 SPRING_DELAY = 125;

static double SPRING_THRESHOLD = 48;

static double SPRING_JUMP = 352;

int handler_spring_init(struct sprite *s) {
  assert_not_null(1, s);
  struct fps_timer *t = fps_timer_create(SPRING_DELAY);

  if (t == NULL) {
    return -1;
  }

  s->data.helper.interaction_timer = t;
  s->animation.type = ANIMATION_FRAME_VERTICAL;
  return 0;
}

int handler_spring_frame(struct sprite *s) {
  assert_not_null(1, s);

  struct fps_timer *t = s->data.helper.interaction_timer;
  fps_timer_iterate(t);

  if (fps_timer_done(t)) {
    sprite_animation_set_frame_vertical(s, 0, 0, 0);
  }

  return 0;
}

int handler_spring_hit(struct sprite *s) {
  struct player *p = g_game.player;
  assert_not_null(2, p, s);

  if (p->s->vy <= SPRING_THRESHOLD) {
    return 0;
  }

  sound_play(SOUND_SPRING, CHANNEL_PLAYER);
  p->s->vy = -SPRING_JUMP;

  sprite_animation_set_frame_vertical(s, 1, 1, 0);
  fps_timer_reset(s->data.helper.interaction_timer);
  return 0;
}

int handler_spring_destroy(struct sprite *s) {
  assert_not_null(1, s);

  fps_timer_destroy(&s->data.helper.interaction_timer);
  return 0;
}
