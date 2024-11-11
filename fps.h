#ifndef FPS_H
#define FPS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// this idea is adapted from arturgunyan's codebase. They link the following
// helpful article, which also made us understand why a max frame time is so
// important: https://gafferongames.com/post/fix_your_timestep/
// To summarize, if processing a frame takes more than MAX_FRAME_TIME,
// the state of the game changes by more than our code can update. So we
// essentially "slow down" the game, in this case, by treating longer duration
// frames as shorter duration frames.
enum {
  // FRAME_RATE is the upper bound on the frames per second (fps) rendered
  // during the game loop.
  FRAME_RATE = 48,
  FRAME_TIME = 1000 / FRAME_RATE,
  MIN_FRAME_RATE = 24, // frames per second
  MAX_FRAME_TIME = 1000 / MIN_FRAME_RATE
};

// init_fps_cap initializes the state required to maintain an upper bound on
// the frames per second (fps) that are rendered.
void fps_init(void);

// fps_iterate should be called at every iteration of the game loop, this
// delays the game loop to maintain an upper bound on the frames per second
// (fps) that are rendered.
void fps_iterate(void);

// fps_frame_time returns the elapsed frame time (or MAX_FRAME_TIME if the
// elapsed frame time is higher than MAX_FRAME_TIME).
Uint64 fps_frame_time(void);

// fps_get returns the current frame rate in frames per second
Uint64 fps_get(void);

// timer is a structure that acts like a mechanical timer going off after a
// given delay. It is used to process multiple delay based gameplay and ui
// mechanics across the game.
struct fps_timer {
  // delay is the wait time the fps_timer is initially set to, in milliseconds
  Uint64 delay;
  // time_left is how much time is currently left of the originally set
  // fps_timer delay, in milliseconds
  Uint64 time_left;
};

// fps_timer_create creates a new fps_timer and sets a delay value for it.
// returns NULL on failure.
struct fps_timer *fps_timer_create(const Uint64 delay);

// fps_timer_destroy frees the memory associated with a fps_timer and sets the
// pointer to NULL
void fps_timer_destroy(struct fps_timer **pt);

// fps_timer_iterate updates the time_left of the fps_timer based on the elapsed
// fps_frame_time
void fps_timer_iterate(struct fps_timer *t);

// fps_timer_done returns true if there is no time left in the fps_timer, false
// otherwise
bool fps_timer_done(struct fps_timer *t);

// fps_timer_reset sets the fps_timer back to the original delay
void fps_timer_reset(struct fps_timer *t);

#endif // FPS_H
