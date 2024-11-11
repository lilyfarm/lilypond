#include "base.h"
#include "camera.h"

#include "test.h"
#include <stdlib.h>

// The camera should never move if the level is 1 screen wide
static void test_camera_single_screen(void) {
  SDL_Rect c;
  struct sprite s;
  s.x = 0;
  s.y = 0;

  // level width and height
  const size_t w = 1;
  const size_t h = 1;

  camera_create(&c, &s, w, h);

  assert(c.x == 0);
  assert(c.w == COLUMN_COUNT);
  assert(c.y == 0);
  assert(c.h == ROW_COUNT);

  // move sprite to the end of the screen
  s.x = LEVEL_WIDTH;
  camera_update(&c, &s, w, h);

  assert(c.x == 0);

  // move sprite to the bottom of the screen
  s.x = 0;
  s.y = LEVEL_HEIGHT;
  camera_update(&c, &s, w, h);

  assert(c.y == 0);

  // move sprite to the middle of the screen
  s.y = 0;
  s.x = (double)LEVEL_WIDTH / 2;
  camera_update(&c, &s, w, 1);
  assert(c.x == 0);

  s.x = 0;
  s.y = (double)LEVEL_HEIGHT / 2;
  camera_update(&c, &s, w, 1);
  assert(c.y == 0);

  SDL_Rect dst;
  dst.x = 42;
  dst.y = 42;
  camera_map(&c, &dst);

  assert(dst.x == 42);
  assert(dst.y == 42);
}

// Tests the horizontal scrolling of the camera
static void test_camera_double_screen_x(void) {
  SDL_Rect c;
  struct sprite s;
  s.x = 0;
  s.y = 0;

  // level width and height
  size_t w = 2;
  size_t h = 1;

  camera_create(&c, &s, w, h);

  assert(c.x == 0);
  assert(c.w == COLUMN_COUNT);

  SDL_Rect dst;
  dst.x = 42;
  camera_map(&c, &dst);

  assert(dst.x == 42);

  // move sprite to the end of the screen
  s.x = 2 * LEVEL_WIDTH;
  camera_update(&c, &s, w, h);
  // should not scroll beyond the first level width, otherwise we will not
  // render anything on the right edge of the screen.
  assert(c.x == LEVEL_WIDTH);

  camera_map(&c, &dst);
  // should be to the left of the start of the screen
  assert(dst.x < 0);

  dst.x = LEVEL_WIDTH + 1;
  camera_map(&c, &dst);
  // should now be on the screen
  assert(dst.x > 0);
  assert(dst.x < LEVEL_WIDTH);
}

// Tests the vertical scrolling of the camera
static void test_camera_double_screen_y(void) {
  SDL_Rect c;
  struct sprite s;
  s.x = 0;
  s.y = 0;

  // level width and height
  size_t w = 1;
  size_t h = 2;

  camera_create(&c, &s, w, h);

  assert(c.y == 0);
  assert(c.h == ROW_COUNT);

  SDL_Rect dst;
  dst.y = 42;
  camera_map(&c, &dst);

  assert(dst.y == 42);

  // move sprite to the bottom of the screen
  s.y = 2 * LEVEL_HEIGHT;
  camera_update(&c, &s, w, h);
  // should not scroll beyond the first level height, otherwise we will not
  // render anything on the right edge of the screen.
  assert(c.y == LEVEL_HEIGHT);

  camera_map(&c, &dst);
  // should be to the top of the top of the screen
  assert(dst.y < 0);

  dst.y = LEVEL_HEIGHT + 1;
  camera_map(&c, &dst);
  // should now be on the screen
  assert(dst.y > 0);
  assert(dst.y < LEVEL_HEIGHT);
}

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  RUN_TEST(test_camera_single_screen);
  RUN_TEST(test_camera_double_screen_x);
  RUN_TEST(test_camera_double_screen_y);

  return EXIT_SUCCESS;
}
