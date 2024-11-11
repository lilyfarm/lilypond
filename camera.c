#include "camera.h"

#include "safe.h"
#include "util.h"
#include <assert.h>

void camera_create(SDL_Rect *c, const struct sprite *s, const size_t w,
                   const size_t h) {
  c->w = COLUMN_COUNT;
  c->h = ROW_COUNT;
  c->y = 0;
  c->x = 0;
  camera_update(c, s, w, h);
}

void camera_update(SDL_Rect *c, const struct sprite *s, const size_t w,
                   const size_t h) {
  size_t left_half_x = (COLUMN_COUNT / 2) * SPRITE_SIZE;
  size_t right_half_x = (w - 1) * COLUMN_COUNT * SPRITE_SIZE + left_half_x;
  // if the amount of "level" to the left of the sprite is less than half of the
  // screen, don't move the camera further left.
  if (s->x <= left_half_x) {
    c->x = 0;
    // if the amount of "level" to the right of the sprite is less than half of
    // the screen, don't move the camera further right.
  } else if (s->x >= right_half_x) {
    c->x = (w - 1) * COLUMN_COUNT * SPRITE_SIZE;
    // otherwise, orient the camera's x position based on the sprite's position
  } else {
    c->x = s->x - left_half_x;
  }

  // make sure that x is still within the bounds of the level as a sanity check
  assert(c->x >= 0 &&
         c->x <= size_t_to_int(w - 1) * COLUMN_COUNT * SPRITE_SIZE);

  // do the same on the y-axis
  size_t top_half_y = (ROW_COUNT / 2) * SPRITE_SIZE;
  size_t bottom_half_y = (h - 1) * ROW_COUNT * SPRITE_SIZE + top_half_y;
  // if the amount of "level" above the sprite is less than half of the
  // screen, don't move the camera further up.
  if (s->y <= top_half_y) {
    c->y = 0;
    // if the amount of "level" below the sprite is less than half of the
    // screen, don't move the camera further right.
  } else if (s->y >= bottom_half_y) {
    c->y = (h - 1) * ROW_COUNT * SPRITE_SIZE;
    // otherwise, orient the camera's y position based on the sprite's position
  } else {
    c->y = s->y - top_half_y;
  }

  // make sure that y is still within the bounds of the level as a sanity check
  assert(c->y >= 0 && c->y <= size_t_to_int(h - 1) * ROW_COUNT * SPRITE_SIZE);
}

bool camera_map(const SDL_Rect *c, SDL_Rect *dst) {
  dst->x -= c->x;
  dst->y -= c->y;

  // if the right border of the rect is more right than the beginning of the
  // screen, and the left border of the rect is more left than the end of the
  // screen, then it is in the screen.
  bool okay_x = dst->x + dst->w > 0 && dst->x < COLUMN_COUNT * SPRITE_SIZE;
  // if the bottom border is below the top of the screen, and the top border is
  // above the bottom of the screen, then it is in the screen
  bool okay_y = dst->y + dst->w > 0 && dst->y < ROW_COUNT * SPRITE_SIZE;

  return okay_x && okay_y;
}
