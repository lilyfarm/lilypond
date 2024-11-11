#include "fps.h"

#include "safe.h"
#include "test.h"

static void test_timer_simple(void) {
  struct fps_timer *t = fps_timer_create(0);
  assert(fps_timer_done(t));
  fps_timer_destroy(&t);
}

static void test_timer_delay(void) {
  struct fps_timer *t = fps_timer_create(10000); // 10 seconds

  fps_init();
  SDL_Delay(1); // delay of 1 ms
  fps_iterate();
  fps_timer_iterate(t);

  assert(t->time_left < t->delay);
  assert(!fps_timer_done(t));

  fps_timer_destroy(&t);
}

static void test_timer_done(void) {
  struct fps_timer *t = fps_timer_create(1);

  fps_init();
  SDL_Delay(1);
  fps_iterate();
  fps_timer_iterate(t);

  assert(fps_timer_done(t));
}

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  RUN_TEST(test_timer_simple);
  RUN_TEST(test_timer_delay);
  RUN_TEST(test_timer_done);

  return EXIT_SUCCESS;
}
