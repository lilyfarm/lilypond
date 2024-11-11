#include "scene.h"

#include "fps.h"
#include "safe.h"
#include "state.h"

struct fps_timer *end_timer;

const Uint64 END_DELAY = 250;

// handlers for SCENE_END
static int scene_end_render(struct scene_state *s) {
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

  x = 120;
  y = 250;

  const char *msg1 = "Thank you for playing!";

  y = 220;

  TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR);

  if (scene_raw_text(s->renderer, s->font, msg1, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  if (!fps_timer_done(end_timer)) {
    return 0;
  }

  const char *prompt = "Press ENTER to continue";

  y = 450;

  if (scene_raw_text(s->renderer, s->font, prompt, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  return 0;
}

static int scene_end_iterate(void) {
  assert_not_null(1, end_timer);

  fps_timer_iterate(end_timer);

  if (!fps_timer_done(end_timer)) {
    return 0;
  }

  const Uint8 *k = g_prog.keys;

  if (k[SDL_SCANCODE_RETURN]) {
    scene_change(SCENE_MENU);
  }

  return 0;
}

static int scene_end_create(void) {
  end_timer = fps_timer_create(END_DELAY);

  if (end_timer == NULL) {
    return -1;
  }

  return 0;
}

static int scene_end_destroy(void) {
  assert_not_null(1, end_timer);
  fps_timer_destroy(&end_timer);
  return 0;
}

void scene_end_load(void) {
  struct scene *s = g_scenes[SCENE_END];
  s->create = scene_end_create;
  s->destroy = scene_end_destroy;
  s->iterate = scene_end_iterate;
  s->render = scene_end_render;
}
