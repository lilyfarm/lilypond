#include "fps.h"

#include "assert.h"
#include "safe.h"

static Uint64 _prev_time = 0;
static Uint64 _start_time = 0;
static Uint64 _curr_time = 0;
static Uint64 _elapsed_time = 0;
static Uint64 _frame_count = 0;

void fps_init(void) { _start_time = _prev_time = SDL_GetTicks64(); }

void fps_iterate(void) {
  _curr_time = SDL_GetTicks64();
  Uint64 elapsed_time = _curr_time - _prev_time;

  if (elapsed_time < FRAME_TIME) {
    // delay until we reach the desired frame rate (per second)
    SDL_Delay(FRAME_TIME - elapsed_time);
    _curr_time = SDL_GetTicks64();
  }

  assert(_curr_time > _prev_time);
  _elapsed_time = _curr_time - _prev_time;
  _prev_time = _curr_time;
  _frame_count += 1;
}

Uint64 fps_frame_time(void) {
  if (_elapsed_time > MAX_FRAME_TIME) {
    return MAX_FRAME_TIME;
  }
  return _elapsed_time;
}

Uint64 fps_get(void) {
  double elapsed_seconds = (_prev_time - _start_time) / 1000.0;
  Uint64 fps = _frame_count / elapsed_seconds;
  return fps;
}

struct fps_timer *fps_timer_create(const Uint64 delay) {
  struct fps_timer *t = malloc(sizeof(struct fps_timer));
  if (t == NULL) {
    LOG_ERROR("could not allocate fps_timer");
    return NULL;
  }

  t->delay = delay;
  t->time_left = delay;
  return t;
}

void fps_timer_destroy(struct fps_timer **pt) {
  struct fps_timer *t = *pt;
  free(t);
  *pt = NULL;
}

void fps_timer_iterate(struct fps_timer *t) {
  const Uint64 dt = fps_frame_time();
  t->time_left = dt > t->time_left ? 0 : t->time_left - dt;
}

bool fps_timer_done(struct fps_timer *t) { return t->time_left == 0; }

void fps_timer_reset(struct fps_timer *t) { t->time_left = t->delay; }
