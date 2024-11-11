#include "sprite.h"

#include "safe.h"
#include "state.h"

// animation_set sets the animation state of the sprite
static void animation_set(struct sprite *s, const int start, const int end,
                          const int fps, const int type) {
  assert_not_null(1, s);
  struct animation *a = &s->animation;
  a->type = type;
  a->frame_start = start;
  a->frame_end = end;

  // in milliseconds
  if (fps == 0) {
    // to prevent a divide by zero error
    a->frame_delay = 0;
  } else {
    a->frame_delay = 1000 / fps;
  }

  // if the current frame is not in the range
  if (a->frame < start || a->frame > end) {
    // set it to the start frame
    a->frame = start;
  }

  // if the frame_delay_counter is not in the range
  if (a->frame_delay_counter > a->frame_delay || a->frame_delay_counter < 0) {
    // set it to the current frame_delay
    a->frame_delay_counter = a->frame_delay;
  }
}

void sprite_animation_set_frame(struct sprite *s, const int start,
                                const int end, const int fps) {
  animation_set(s, start, end, fps, ANIMATION_FRAME);
}

void sprite_animation_set_frame_vertical(struct sprite *s, const int start,
                                         const int end, const int fps) {
  animation_set(s, start, end, fps, ANIMATION_FRAME_VERTICAL);
}

void sprite_animation_set_flip(struct sprite *s, const int frame,
                               const int fps) {
  animation_set(s, frame, frame, fps, ANIMATION_FLIP);
}

int sprite_init(struct sprite *s, const enum sprite_id id) {
  assert_not_null(1, s);
  s->type = g_sprite_types[id];
  // all are ints, multiple assignment seems fine here
  s->x = s->y = s->vx = s->vy = 0;
  s->removed = false;

  // initialize the animation
  s->animation.flip = SDL_FLIP_NONE;
  s->animation.frame_delay_counter = 0;
  s->animation.type = ANIMATION_FRAME;
  s->animation.alpha = SDL_ALPHA_OPAQUE; // 255 on the current SDL2 version
  // default frame animation that starts at the first frame, ends at the first
  // frame, and has a frames-per-second of zero i.e changes zero frames per
  // second.
  sprite_animation_set_frame(s, 0, 0, 0);
  // since we are initializing this sprite, also call its initialization
  // sprite handler.
  if (s->type->init_handler(s) != 0) {
    LOG_ERROR("failed to initialize sprite with id: %d", s->type->id);
    return -1;
  }

  return 0;
}
