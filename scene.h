#ifndef SCENE_H
#define SCENE_H

#include "base.h"

#include "message.h"
#include <SDL2/SDL_ttf.h>

// for use with scene_raw_text_with_color
#define COLOR_MAGENTA ((SDL_Color){128, 0, 128, 255})
#define COLOR_YELLOW ((SDL_Color){128, 128, 0, 255})
#define COLOR_CYAN ((SDL_Color){0, 128, 128, 255})

enum {
  // FONT_SIZE is the size of the font for our game messages. We are using 8-bit
  // fonts.
  DEFAULT_FONT_SIZE = 8
};

// scene_state contains all the state required to provide the "view" for a scene
// i.e the state for rendering images, text, and (in the future) the state for
// sound etc.
struct scene_state {
  // We will be using this to render everything in the game
  SDL_Renderer *renderer;
  // The main SDL window
  SDL_Window *window;
  // The sprite sheet
  SDL_Texture *sprite_sheet;
  // The font we will be using to render the text for our game messages
  TTF_Font *font;
};

// scene_handler is to store the functionality of creating, iterating, and
// destroying a scene.
typedef int (*scene_handler)(void);

// scene_render_handler is to store the functionality for rendering a scene
typedef int (*scene_render_handler)(struct scene_state *);

// scene_id contains the types of scenes we have in the game
enum scene_id {
  // SCENE_NONE is just a blank screen with the text "No scene loaded"
  SCENE_NONE = 0,
  // SCENE_INTRO is the intro of the game
  SCENE_INTRO,
  // SCENE_END is the end of the game
  SCENE_END,
  // SCENE_MENU is the main menu of the game
  SCENE_MENU,
  // SCENE_OPTIONS is the options menu of the game
  SCENE_OPTIONS,
  // SCENE_GAME is the scene where most of the game play occurs. This is where
  // worlds are generated, and the player is moved.
  SCENE_GAME,
  // SCENE_ACKNOWLEDGEMENTS displays acknowledgements for public domain content
  // we used in the game
  SCENE_ACKNOWLEDGEMENTS,
  // SCENE_COUNT should always be the final element as it is the size used
  // to initialize the g_scenes array.
  SCENE_COUNT,
};

// scene is used to keep track of multiple "scenes" in the game and how to
// render them
struct scene {
  // creates any state necessary for the scene
  scene_handler create;
  // renders all objects in the scene onto the screen, this updates the "view"
  // of the scene
  scene_render_handler render;
  // processes all objects in the scene, this updates the "model" of the scene
  scene_handler iterate;
  // frees up any state taken up by the scene
  scene_handler destroy;
};

// scene_state_create creates all the necessary state (window, sprites, fonts)
// for rendering all scenes in the game. Returns NULL on failure.
struct scene_state *scene_state_create(void);

// scene_state_destroy deallocates the state created by create_scene_state
void scene_state_destroy(struct scene_state **ps);

// scene_state_clear paints the screen black
int scene_state_clear(struct scene_state *s);

// scene_state_present internally calls SDL_RenderPresent
void scene_state_present(struct scene_state *s);

// scene_state_fullscreen toggles full screen. It sets full screen if fullscreen
// is true, and unsets it otherwise. Returns 0 on success, -1 on failure.
int scene_state_fullscreen(struct scene_state *s, bool fullscreen);

// scene_change destroys the current scene, creates a new scene
// corresponding to id, and sets it as the current scene. Returns 0 on success,
// -1 on error.
int scene_change(enum scene_id id);

// scene_raw_text renders `text` using font `font` and wrap_length
// `wrap_length`. The `wrap_length` can be 0 if you only want new line
// characters to jump over to the new line. The text is rendered at co-ordinated
// (x, y). Returns 0 on success, -1 on failure.
int scene_raw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                   const Uint32 wrap_length, const int x, const int y);

// scene_raw_text_with_color works like scene_raw_text but also takes the color
int scene_raw_text_with_color(SDL_Renderer *renderer, TTF_Font *font,
                              const char *text, const Uint32 wrap_length,
                              const int x, const int y, const SDL_Color color);

// scene_message renders a message on the screen. Returns 0 on success, -1 on
// failure.
int scene_message(struct scene_state *s, const struct message *msg);

// scene_player_status shows some information relevant to the player in the
// status bar.
int scene_player_status(struct scene_state *s, const unsigned int lives,
                        const unsigned int coins);

// ------------------------------------
// -- loaders for scenes in the game --
// ------------------------------------

void scene_game_load(void);
void scene_none_load(void);
void scene_intro_load(void);
void scene_end_load(void);
void scene_menu_load(void);
void scene_options_load(void);
void scene_acknowledgements_load(void);

#endif // SCENE_H
