#include "scene.h"

#include "file_chooser.h"
#include "fps.h"
#include "safe.h"
#include "sound.h"
#include "state.h"

#define BUF_LEN 256

static const Uint64 DELAY = 250;
static const Uint64 INITIAL_DELAY = 250;

// we want some delay before allowing user input when the menu loads initially.
// Otherwise, if the user has pressed enter, say to dismiss a splash screen, it
// might accidentally trigger an option selection in the menu.
static struct fps_timer *init_timer = NULL;

// we want some delay while scrolling through menu options, otherwise they
// scroll too quickly for the menu to be usable.
static struct fps_timer *scroll_timer = NULL;

enum menu {
  MENU_START = 0,
  MENU_LOAD_LEVEL,
  MENU_OPTIONS,
  MENU_ACKNOWLEDGEMENTS,
  MENU_EXIT,
  MENU_COUNT
};

static char _items[MENU_COUNT][BUF_LEN] = {"START", "LOAD LEVEL", "OPTIONS",
                                           "ACKNOWLEDGEMENTS", "EXIT"};
static int _item = 0;

static int scene_menu_create(void) {
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

static int scene_menu_destroy(void) {
  assert_not_null(2, init_timer, scroll_timer);
  fps_timer_destroy(&init_timer);
  fps_timer_destroy(&scroll_timer);
  return 0;
}

static int scene_menu_iterate(void) {
  const Uint8 *k = g_prog.keys;

  fps_timer_iterate(init_timer);
  fps_timer_iterate(scroll_timer);

  if (!fps_timer_done(init_timer)) {
    return 0;
  }

  if (k[SDL_SCANCODE_ESCAPE]) {
    return scene_change(SCENE_INTRO);
  }

  if (k[SDL_SCANCODE_RETURN]) {
    // play the sound effect
    sound_play(SOUND_MENU_SELECT, CHANNEL_UI);

    switch (_item) {
    case MENU_START:
      g_game.custom_level_path = NULL;
      if (scene_change(SCENE_GAME) != 0) {
        goto error_out;
      }
      return 0;
    case MENU_LOAD_LEVEL:
#ifdef _WIN32
      LOG_ERROR("load level not supported on windows");
      return 0;
#else
      // if we already have a custom level set, destroy it
      if (g_game.custom_level_path != NULL) {
        free(g_game.custom_level_path);
        g_game.custom_level_path = NULL;
      }

      g_game.custom_level_path = file_chooser_open();
      if (g_game.custom_level_path == NULL) {
        LOG_INFO("could not load custom level");
        break;
      }

      LOG_INFO("loading level: %s", g_game.custom_level_path);

      if (scene_change(SCENE_GAME) != 0) {
        free(g_game.custom_level_path);
        g_game.custom_level_path = NULL;

        goto error_out;
      }

      return 0;
#endif
    case MENU_OPTIONS:
      if (scene_change(SCENE_OPTIONS) != 0) {
        goto error_out;
      }
      return 0;
    case MENU_ACKNOWLEDGEMENTS:
      if (scene_change(SCENE_ACKNOWLEDGEMENTS) != 0) {
        goto error_out;
      }
      return 0;
    case MENU_EXIT:
      g_prog.state = PROG_EXIT;
      return 0;
    default:
      LOG_ERROR("unknown menu option");
      goto error_out;
    }
  }

  if (!fps_timer_done(scroll_timer)) {
    return 0;
  }

  if (k[SDL_SCANCODE_DOWN] || k[SDL_SCANCODE_UP]) {
    // play the sound effect either way
    sound_play(SOUND_MENU_MOVE, CHANNEL_UI);
    fps_timer_reset(scroll_timer);
  }

  if (k[SDL_SCANCODE_DOWN]) {
    _item = SDL_min(_item + 1, MENU_COUNT - 1);
#ifdef _WIN32
    if (_item == 1) {
      _item = 2;
    }
#endif
  } else if (k[SDL_SCANCODE_UP]) {
    _item = SDL_max(_item - 1, 0);
#ifdef _WIN32
    if (_item == 1) {
      _item = 0;
    }
#endif
  }

  return 0;

error_out:
  g_prog.state = PROG_ERROR;
  return -1;
}

static int scene_menu_render(struct scene_state *s) {
  assert_not_null(2, s, s->font);

  char buf[BUF_LEN] = {0};
  int remaining = BUF_LEN;

  register size_t i;

  char *token = buf;
  for (i = 0; i < MENU_COUNT; i++) {
#ifdef _WIN32
    if (i == 1) {
      continue;
    }
#endif

    const char *format;

    if (i == (size_t)_item && fps_timer_done(init_timer)) {
      format = "> %s\n\n";
    } else {
      format = "  %s\n\n";
    }

    int written = snprintf(token, remaining, format, _items[i]);
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

  x = 200;
  y = 120;

  const char *title = "LILYPOND";

  if (scene_raw_text_with_color(s->renderer, s->font, title, 0, x, y,
                                COLOR_MAGENTA) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  const char *instructions = "UP / DOWN arrow keys to move.\n\nENTER to "
                             "select.\n\nESC to return to title.";

  x = 100;
  y = 500;

  if (scene_raw_text(s->renderer, s->font, instructions, 0, x, y) != 0) {
    return -1;
  }

  return 0;
}

void scene_menu_load(void) {
  struct scene *s = g_scenes[SCENE_MENU];
  s->create = scene_menu_create;
  s->destroy = scene_menu_destroy;
  s->iterate = scene_menu_iterate;
  s->render = scene_menu_render;
}
