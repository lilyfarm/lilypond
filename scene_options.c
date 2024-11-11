#include "scene.h"

#include "fps.h"
#include "render.h"
#include "safe.h"
#include "sound.h"
#include "state.h"

#define BUF_LEN 256

// OPTIONS_SOUND_ENABLED is in sound.c
extern bool OPTIONS_SOUND_ENABLED;
static bool OPTIONS_FULLSCREEN_ENABLED = false;

static const Uint64 DELAY = 250;
static const Uint64 INITIAL_DELAY = 250;

// we want some delay before allowing user input when the menu loads initially.
// Otherwise, if the user has pressed enter, say to dismiss a splash screen, it
// might accidentally trigger an option selection in the menu.
static struct fps_timer *init_timer = NULL;

// we want some delay while scrolling through menu options, otherwise they
// scroll too quickly for the menu to be usable.
static struct fps_timer *scroll_timer = NULL;

static int _item = 0;

enum menu { OPTIONS_SOUND = 0, OPTION_FULLSCREEN, OPTIONS_COUNT };

static int scene_options_create(void) {
  init_timer = fps_timer_create(INITIAL_DELAY);
  if (init_timer == NULL) {
    return -1;
  }

  scroll_timer = fps_timer_create(DELAY);
  if (scroll_timer == NULL) {
    fps_timer_destroy(&init_timer);
    return -1;
  }
  return 0;
}

static int scene_options_destroy(void) {
  assert_not_null(2, init_timer, scroll_timer);
  fps_timer_destroy(&init_timer);
  fps_timer_destroy(&scroll_timer);
  return 0;
}

static int scene_options_iterate(void) {
  const Uint8 *k = g_prog.keys;

  fps_timer_iterate(init_timer);
  fps_timer_iterate(scroll_timer);

  if (!fps_timer_done(init_timer)) {
    return 0;
  }

  if (k[SDL_SCANCODE_ESCAPE]) {
    return scene_change(SCENE_MENU);
  }

  if (!fps_timer_done(scroll_timer)) {
    return 0;
  }

  if (k[SDL_SCANCODE_RETURN]) {
    // play the sound effect
    sound_play(SOUND_MENU_SELECT, CHANNEL_UI);
    fps_timer_reset(scroll_timer);

    switch (_item) {
    case OPTION_FULLSCREEN:
      OPTIONS_FULLSCREEN_ENABLED = OPTIONS_FULLSCREEN_ENABLED ? false : true;
      if (render_fullscreen(OPTIONS_FULLSCREEN_ENABLED) != 0) {
        return -1;
      }
      break;
    case OPTIONS_SOUND:
      OPTIONS_SOUND_ENABLED = OPTIONS_SOUND_ENABLED ? false : true;
      break;
    }
  }

  if (k[SDL_SCANCODE_DOWN] || k[SDL_SCANCODE_UP]) {
    // play the sound effect either way
    sound_play(SOUND_MENU_MOVE, CHANNEL_UI);
    fps_timer_reset(scroll_timer);
  }

  if (k[SDL_SCANCODE_DOWN]) {
    _item = SDL_min(_item + 1, OPTIONS_COUNT - 1);
  } else if (k[SDL_SCANCODE_UP]) {
    _item = SDL_max(_item - 1, 0);
  }

  return 0;
}

static int scene_options_render(struct scene_state *s) {
  assert_not_null(2, s, s->font);

  char buf[BUF_LEN] = {0};
  int remaining = BUF_LEN;

  register size_t i;

  char items[OPTIONS_COUNT][BUF_LEN];

  strcpy(items[0], OPTIONS_SOUND_ENABLED ? "SOUND: ON" : "SOUND: OFF");
  strcpy(items[1],
         OPTIONS_FULLSCREEN_ENABLED ? "FULL SCREEN: ON" : "FULL SCREEN: OFF");

  char *token = buf;
  for (i = 0; i < OPTIONS_COUNT; i++) {
    const char *format;

    if (i == (size_t)_item && fps_timer_done(init_timer)) {
      format = "> %s\n\n";
    } else {
      format = "  %s\n\n";
    }

    int written = snprintf(token, remaining, format, items[i]);
    assert(written >= 0 || written < remaining);
    remaining -= written;
    token += written;
  }

  buf[BUF_LEN - 1] = '\0';

  int x = 230;
  int y = 300;

  if (scene_raw_text(s->renderer, s->font, buf, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  x = 100;
  y = 120;

  const char *title = "OPTIONS";

  if (scene_raw_text(s->renderer, s->font, title, 0, x, y) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  const char *instructions = "UP / DOWN arrow keys to move.\n\nENTER to "
                             "select.\n\nESC to return to Main Menu.";

  x = 100;
  y = 500;

  if (scene_raw_text(s->renderer, s->font, instructions, 0, x, y) != 0) {
    return -1;
  }

  return 0;
}

void scene_options_load(void) {
  struct scene *s = g_scenes[SCENE_OPTIONS];
  s->create = scene_options_create;
  s->destroy = scene_options_destroy;
  s->iterate = scene_options_iterate;
  s->render = scene_options_render;
}
