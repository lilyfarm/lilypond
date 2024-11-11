#include "scene.h"

#include "fps.h"
#include "render.h"
#include "safe.h"
#include "sound.h"
#include "state.h"

#define BUF_LEN 2048

static size_t _page = 0;

static int scene_acknowledgements_create(void) { return 0; }

static int scene_acknowledgements_destroy(void) { return 0; }

static int scene_acknowledgements_iterate(void) {
  const Uint8 *k = g_prog.keys;

  if (k[SDL_SCANCODE_ESCAPE]) {
    return scene_change(SCENE_MENU);
  }

  if (k[SDL_SCANCODE_RIGHT]) {
    _page = 1;
  }

  if (k[SDL_SCANCODE_LEFT]) {
    _page = 0;
  }

  if (k[SDL_SCANCODE_LEFT] || k[SDL_SCANCODE_RIGHT]) {
    // play the sound effect either way
    sound_play(SOUND_MENU_MOVE, CHANNEL_UI);
  }

  return 0;
}

static int scene_acknowledgements_render(struct scene_state *s) {
  assert_not_null(2, s, s->font);

  const char *acknowledgements_page1 =
      "The source code for lilypond is written in the C "
      "programming language. It uses the SDL2 library and emscripten."
      "It is distributed under the GNU GPL "
      "v3 License.\n\n"
      "lilypond uses \"Press Start 2P\" font by "
      "Cody \"CodeMan38\" "
      "Boisclair: zone38.net/font/#pressstart (SIL Open Font License).\n\n"
      "The implementation was inspired by 'sdl-platformer' by "
      "artureganyan: github.com/artureganyan/sdl-platformer (MIT License).\n\n"
      "All other content in this game has been used / adapted from public "
      "domain (Creative Commons CC0) sources from Open Game "
      "Art listed on Page 2.";

  const char *acknowledgements_page2 =
      "1) 'Simple Broad Purpose Tileset' by Sharm "
      "(Lanea Zimmerman), surt (Carl Olsson), vk | CC0 License | "
      "opengameart.org/content/simple-broad-purpose-tileset.\n\n"
      "2) 'Simple Pimples Water Animation' by Spring Spring | CC0 License | "
      "opengameart.org/content/simples-pimples-water-animation.\n\n"
      "3) 'Cat Sprites' by Shepardskin | CC0 License | "
      "opengameart.org/content/cat-sprites.\n\n"
      "4) '50 CC0 retro / synth SFX' by rubberduck | CC0 License | "
      "opengameart.org/content/50-cc0-retro-synth-sfx.\n\n"
      "5) '512 Sound Effects (8 bit style)' by SubspaceAudio | CC0 License | "
      "opengameart.org/content/512-sound-effects-8-bit-style\n\n"
      "lilyfarm thanks all these amazing creators who released their art into "
      "the public domain and made this project possible";

  int x = 20;
  int y = 10;

  if (scene_raw_text_with_color(
          s->renderer, s->font,
          _page == 0 ? acknowledgements_page1 : acknowledgements_page2,
          LEVEL_WIDTH * SIZE_FACTOR - 40, x, y, COLOR_CYAN) != 0) {
    // All errors are logged by scene_raw_text
    return -1;
  }

  const char *instructions =
      "*************************\n\nESC to return to Main Menu.";

  x = 50;
  y = 490;

  if (scene_raw_text(s->renderer, s->font, instructions, 0, x, y) != 0) {
    return -1;
  }

  const char *page = _page == 0
                         ? "You are on page 1. Press RIGHT to go to page 2."
                         : "You are on page 2. Press LEFT to go to page 1.";

  y = 550;

  if (scene_raw_text(s->renderer, s->font, page, LEVEL_WIDTH * SIZE_FACTOR - 20,
                     x, y) != 0) {
    return -1;
  }

  return 0;
}

void scene_acknowledgements_load(void) {
  struct scene *s = g_scenes[SCENE_ACKNOWLEDGEMENTS];
  s->create = scene_acknowledgements_create;
  s->destroy = scene_acknowledgements_destroy;
  s->iterate = scene_acknowledgements_iterate;
  s->render = scene_acknowledgements_render;
}
