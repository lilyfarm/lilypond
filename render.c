#include "render.h"

#include "fps.h"
#include "safe.h"
#include "scene.h"
#include "state.h"

static struct scene_state *_state = NULL;

int render_create(void) {
  _state = scene_state_create();
  if (_state == NULL) {
    return -1;
  }

  fps_init(); // initialize the frame rate limiter

  return 0;
}

void render_destroy(void) {
  assert_not_null(1, _state);
  scene_state_destroy(&_state);
}

// render_current_scene renders the scene pointed to by g_game.scene
static int render_current_scene(void) {
  // this is an assert instead of an error since if the current scene is NULL,
  // that is a programming bug, not a runtime bug.
  assert_not_null(2, g_prog.scene, _state);
  return g_prog.scene->render(_state);
}

int render_fullscreen(bool fullscreen) {
  return scene_state_fullscreen(_state, fullscreen);
}

int render_iterate(void) {
  assert_not_null(1, _state);

  // clear the screen to remove the old scene or an earlier iteration of the
  // current scene.
  if (scene_state_clear(_state) != 0) {
    return -1;
  }

  // ------------------------------
  // -- render the current scene --
  // ------------------------------
  if (render_current_scene() != 0) {
    LOG_ERROR("rendering current scene failed");
    return -1;
  }

  scene_state_present(_state); // SDL_RenderPresent

  fps_iterate(); // maintain frame rate

  return 0;
}
