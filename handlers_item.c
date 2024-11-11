#include "handlers.h"

#include "fps.h"
#include "player.h"
#include "safe.h"
#include "sound.h"
#include "state.h"

static const Sint64 DISAPPEAR_DELAY = 150;

int handler_item_init(struct sprite *s) {
  assert_not_null(1, s);
  s->data.item.collected = false;
  return 0;
}

int handler_item_frame(struct sprite *s) {
  assert_not_null(1, s);
  bool collected = s->data.item.collected;

  if (!collected || s->removed) {
    return 0;
  }

  // item was collected
  const double dt = fps_frame_time();
  Sint64 *alpha = &s->animation.alpha;
  *alpha -= (255 / (double)DISAPPEAR_DELAY) * dt;

  if (*alpha < 0) {
    *alpha = 0;
    s->removed = true;
  }

  return 0;
}

int handler_item_hit(struct sprite *s) {
  bool *collected = &s->data.item.collected;
  if ((*collected)) {
    return 0;
  }

  *collected = true;

  switch (s->type->id) {
  case SPRITE_COIN:
    g_game.player->coins++;
    if (sound_play(SOUND_COIN, CHANNEL_SPRITE) != 0) {
      return -1;
    }
    break;
  case SPRITE_EXTRA_LIFE:
    g_game.player->lives++;
    if (sound_play(SOUND_EXTRA_LIFE, CHANNEL_SPRITE) != 0) {
      return -1;
    }
    break;
  default:
    LOG_ERROR("could not handle item");
    return -1;
  }

  return 0;
}
