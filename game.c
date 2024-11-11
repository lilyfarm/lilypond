#include "game.h"

#include <assert.h>
#include <string.h>
#include <time.h>

#include "base.h"
#include "camera.h"
#include "level.h"
#include "player.h"
#include "render.h"
#include "safe.h"
#include "scene.h"
#include "sound.h"
#include "state.h"

// game_create initializes the game state, the rendering state (including SDL2
// and its modules), the frame rate regulating system, the level, the camera,
// and the scene.
void game_create(void) {
  // seed the random number generator
  srand(time(NULL));

  assert(render_create() == 0);
  assert(sound_create() == 0);

  // this will be valid for the whole lifetime of the application,
  // according to SDL2 docs, and we should never free it.
  g_prog.keys = SDL_GetKeyboardState(NULL);

  assert(scene_change(SCENE_INTRO) == 0);
  g_prog.state = PROG_GAME_IN;
}

// game_destroy destroys all game and rendering state, and frees up any
// remaining memory allocated on the heap.
static void game_destroy(void) {
  // sound_destroy destroys all the sound state
  sound_destroy();
  // render_destroy destroys all the scene state and quits SDL
  render_destroy();
}

// game_iterate updates the "model" of the current scene
static int game_iterate(void) {
  // this is an assert instead of an error since if the current scene is
  // NULL, that is a programming bug, not a runtime bug.
  assert_not_null(1, g_prog.scene);
  return g_prog.scene->iterate();
}

void game_main_loop(void) {
  enum prog_state *s = &g_prog.state;

  if (*s == PROG_NOT_STARTED) {
    game_create();
    LOG_INFO("initialized the game");
  }

  if (*s == PROG_EXIT || *s == PROG_ERROR) {
    game_destroy();

#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
    return;
#else
    exit(0);
#endif
  }

  // these states should never reach here
  assert(*s != PROG_NOT_STARTED);
  assert(*s != PROG_EXIT);
  assert(*s != PROG_ERROR);

  SDL_Event event;

  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    *s = PROG_EXIT;
    return;
  }

  // -------------------------------
  // -- iterate the current scene --
  // -------------------------------
  if (game_iterate() != 0) {
    LOG_ERROR("game iteration failed");
    *s = PROG_ERROR;
    return;
  }

  // update the model and view of the current scene
  if (render_iterate() != 0) {
    LOG_ERROR("game rendering failed");
    *s = PROG_ERROR;
    return;
  }
}
