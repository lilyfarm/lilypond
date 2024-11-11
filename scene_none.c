#include "scene.h"

#include "safe.h"
#include "state.h"

// handlers for SCENE_NONE
static int scene_none_render(struct scene_state *s) {
  SAFE_UNUSED(s);
  return 0;
}

// handlers for SCENE_NONE
static int scene_none_iterate(void) { return 0; }

void scene_none_load(void) {
  struct scene *s = g_scenes[SCENE_NONE];
  s->create = scene_none_iterate;
  s->destroy = scene_none_iterate;
  s->iterate = scene_none_iterate;
  s->render = scene_none_render;
}
