#include "scene.h"

#include "fps.h"
#include "safe.h"
#include "state.h"

struct fps_timer *intro_timer;

const Uint64 INTRO_DELAY = 250;

// handlers for SCENE_INTRO
static int scene_intro_render(struct scene_state *s) {
  int x = 200;
  int y = 30;

  TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR * 2);

  const char *title = "LILYPOND";

  if (scene_raw_text_with_color(s->renderer, s->font, title, 0, x, y,
                                COLOR_MAGENTA) != 0) {
    return -1;
  }

  TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR * 0.75);

  x = 20;
  y = 550;

  const char *version = "Version 1.0.0 - " __DATE__
                        "\n\nThis is FREE software, it belongs to the people";

  if (scene_raw_text_with_color(s->renderer, s->font, version, 0, x, y,
                                COLOR_CYAN) != 0) {
    return -1;
  }

  TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR);

  x = 20;
  y = 250;

  const char *msg1 = "Dedicated to the memory of";

  if (scene_raw_text(s->renderer, s->font, msg1, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  x = 200;
  y = 275;

  const char *msg2 = "Thich Quang Duc";

  if (scene_raw_text_with_color(s->renderer, s->font, msg2, 0, x, y,
                                COLOR_YELLOW) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  x = 20;
  y = 300;

  const char *msg3 = "and to the workers of the world.";

  if (scene_raw_text(s->renderer, s->font, msg3, LEVEL_WIDTH * SIZE_FACTOR - 40,
                     x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  y = 220;

  TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR);

  if (!fps_timer_done(intro_timer)) {
    return 0;
  }

  const char *prompt = "Press ENTER to continue";

  x = 120;
  y = 450;

  if (scene_raw_text(s->renderer, s->font, prompt, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  return 0;
}

static int scene_intro_iterate(void) {
  assert_not_null(1, intro_timer);

  fps_timer_iterate(intro_timer);

  if (!fps_timer_done(intro_timer)) {
    return 0;
  }

  const Uint8 *k = g_prog.keys;

  if (k[SDL_SCANCODE_RETURN]) {
    scene_change(SCENE_MENU);
  }

  return 0;
}

static int scene_intro_create(void) {
  intro_timer = fps_timer_create(INTRO_DELAY);

  if (intro_timer == NULL) {
    return -1;
  }

  return 0;
}

static int scene_intro_destroy(void) {
  assert_not_null(1, intro_timer);
  fps_timer_destroy(&intro_timer);
  return 0;
}

void scene_intro_load(void) {
  struct scene *s = g_scenes[SCENE_INTRO];
  s->create = scene_intro_create;
  s->destroy = scene_intro_destroy;
  s->iterate = scene_intro_iterate;
  s->render = scene_intro_render;
}
