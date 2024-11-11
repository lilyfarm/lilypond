#include "scene.h"

#include "level.h"
#include "safe.h"
#include "state.h"

#include <SDL2/SDL.h>
#include <errno.h>

static void g_scenes_destroy(void) {
  register size_t i;

  for (i = 0; i < SCENE_COUNT; i++) {
    if (g_scenes[i] != NULL) {
      free(g_scenes[i]);
      g_scenes[i] = NULL;
    }
  }
}

static int g_scenes_create(void) {
  register size_t i;

  for (i = 0; i < SCENE_COUNT; i++) {
    g_scenes[i] = calloc(1, sizeof(struct scene));
    if (g_scenes[i] == NULL) {
      g_scenes_destroy();
      return -1;
    }
  }

  scene_none_load();
  scene_intro_load();
  scene_end_load();
  scene_menu_load();
  scene_game_load();
  scene_options_load();
  scene_acknowledgements_load();

  return 0;
}

struct scene_state *scene_state_create(void) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    goto error_out;
  }

  if (TTF_Init() != 0) {
    goto sdl_error_out;
  }

  struct scene_state *s = calloc(1, sizeof(struct scene_state));
  if (s == NULL) {
    LOG_ERROR("could not allocate memory for scene state");
    goto ttf_error_out;
  }

  if (SDL_CreateWindowAndRenderer(LEVEL_WIDTH * SIZE_FACTOR,
                                  LEVEL_HEIGHT * SIZE_FACTOR +
                                      TEXT_HEIGHT * SIZE_FACTOR,
                                  0, &s->window, &s->renderer) != 0) {
    goto state_error_out;
  }

  SDL_SetWindowTitle(s->window, "lilyGame");

  // ---------------------------
  // -- Load the sprite sheet --
  // ---------------------------

  // the color that should marked transparent in the sprite sheet
  static const Uint8 color_key[] = {90, 82, 104};
  SDL_Surface *surface = SDL_LoadBMP("share/sprites/spritesheet.bmp");
  if (surface == NULL) {
    goto state_error_out;
  }

  if (SDL_SetColorKey(surface, SDL_TRUE,
                      SDL_MapRGB(surface->format, color_key[0], color_key[1],
                                 color_key[2])) != 0) {
    SDL_FreeSurface(surface);
    goto state_error_out;
  }

  s->sprite_sheet = SDL_CreateTextureFromSurface(s->renderer, surface);
  if (s->sprite_sheet == NULL) {
    SDL_FreeSurface(surface);
    goto state_error_out;
  }

  SDL_FreeSurface(surface); // we don't need the surface any more

  // --------------------
  // -- Load the Fonts --
  // --------------------
  s->font = TTF_OpenFont("share/font/PressStart2P.ttf",
                         DEFAULT_FONT_SIZE * SIZE_FACTOR);
  if (s->font == NULL) {
    goto state_error_out;
  }

  // ---------------------
  // -- Load the scenes --
  // ---------------------
  if (g_scenes_create() != 0) {
    goto state_error_out;
  }

  // Everything was successful
  LOG_INFO_VERBOSE("successfully created scene state");
  return s;

state_error_out:
  free(s);
ttf_error_out:
  TTF_Quit();
sdl_error_out:
  SDL_Quit();
error_out:
  LOG_ERROR("Last SDL Error: %s, Last TTF Error: %s", SDL_GetError(),
            TTF_GetError());
  return NULL;
}

void scene_state_destroy(struct scene_state **ps) {
  struct scene_state *s = *ps;
  SDL_DestroyTexture(s->sprite_sheet);
  TTF_CloseFont(s->font);

  SDL_DestroyRenderer(s->renderer); // free up the renderer
  SDL_DestroyWindow(s->window);     // free up the window

  TTF_Quit();
  SDL_Quit();

  free(s);
  *ps = NULL;

  LOG_INFO_VERBOSE("successfully destroyed scene state");
}

int scene_state_clear(struct scene_state *s) {
  assert_not_null(1, s);
  SDL_Renderer *r = s->renderer;
  assert_not_null(1, r);

  if (SDL_SetRenderDrawColor(r, 0, 0, 0, 0) != 0) {
    goto error_out;
  }

  if (SDL_RenderClear(r) != 0) {
    goto error_out;
  }

  return 0;

error_out:
  LOG_ERROR("%s", SDL_GetError());
  return -1;
}

void scene_state_present(struct scene_state *s) {
  assert_not_null(2, s, s->renderer);
  SDL_RenderPresent(s->renderer);
}

int scene_state_fullscreen(struct scene_state *s, bool fullscreen) {
#ifdef __EMSCRIPTEN__
  if (fullscreen) {
    EmscriptenFullscreenStrategy strategy;
    strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
    strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
    if (emscripten_enter_soft_fullscreen("canvas", &strategy) !=
        EMSCRIPTEN_RESULT_SUCCESS) {
      LOG_ERROR("failed to enter full screen");
      return -1;
    }
    return 0;
  }

  if (emscripten_exit_soft_fullscreen() != EMSCRIPTEN_RESULT_SUCCESS) {
    LOG_ERROR("failed to exit full screen");
    return -1;
  }

  return 0;
#else
  Uint32 flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
  if (SDL_SetWindowFullscreen(s->window, flags) != 0) {
    LOG_ERROR("could not toggle fullscreen: %s", SDL_GetError());
    return -1;
  }
#endif

  return 0;
}

int scene_change(enum scene_id id) {
  struct scene *prev = g_prog.scene;
  struct scene *cur = g_scenes[id];

  if (cur->create() != 0) {
    return -1;
  }

  if (prev != NULL) {
    assert(prev->destroy() == 0);
  }

  g_prog.scene = cur;
  return 0;
}
