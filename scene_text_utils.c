#include "scene.h"

#include "fps.h"
#include "message.h"
#include "player.h"
#include "safe.h"

// This file contains utilities for rendering text in scenes. Any functions
// required outside the scope of this file are declared in scene.h

// FG_COLOR is the color of the text being rendered
static const SDL_Color FG_COLOR = {255, 255, 255, 255}; // white
// BG_COLOR is the color *behind* the text being rendered i.e the background
// color of the textbox
static const SDL_Color BG_COLOR = {0, 0, 0, 255}; // black
// BORDER_COLOR is the color of the borders of the textbok
static const SDL_Color BORDER_COLOR = {255, 255, 255, 255}; // white
// BORDER is the size of the text box border. This is an int because SDL_Rect
// stores integers. Same for other similar values below.
static const int BORDER = 1 * SIZE_FACTOR;
// PADDING is the width (in real pixels) of the empty space between the
// border of the text box and where the text actually starts getting rendered.
static const int PADDING = 5 * SIZE_FACTOR;
// STATUS_PADDING is the width (in real pixels) of the empty space between the
// edges of the screen and the status.
static const int STATUS_PADDING = 4 * SIZE_FACTOR;

// text_box draws the text box surrounding the text. Returns 0 on success, -1 on
// failure.
static int text_box(struct scene_state *s) {
  assert_not_null(2, s, s->renderer);

  SDL_Rect b; // the border rect
  b.x = 0;
  b.y = LEVEL_HEIGHT * SIZE_FACTOR;
  b.w = TEXT_WIDTH * SIZE_FACTOR;
  b.h = TEXT_HEIGHT * SIZE_FACTOR;

  if (SDL_SetRenderDrawColor(s->renderer, BORDER_COLOR.r, BORDER_COLOR.g,
                             BORDER_COLOR.b, BORDER_COLOR.a) != 0) {
    goto error_out;
  }

  if (SDL_RenderFillRect(s->renderer, &b) != 0) {
    goto error_out;
  }

  SDL_Rect i; // the internal rect
  i.x = b.x + BORDER;
  i.y = b.y + BORDER;
  i.w = b.w - BORDER * 2;
  i.h = b.h - BORDER * 2;

  if (SDL_SetRenderDrawColor(s->renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b,
                             BG_COLOR.a) != 0) {
    goto error_out;
  }

  if (SDL_RenderFillRect(s->renderer, &i) != 0) {
    goto error_out;
  }

  return 0;

error_out:
  LOG_ERROR("%s", SDL_GetError());
  return -1;
}

int scene_raw_text_with_color(SDL_Renderer *renderer, TTF_Font *font,
                              const char *text, const Uint32 wrap_length,
                              const int x, const int y, const SDL_Color color) {
  assert_not_null(3, renderer, font, text);
  SDL_Surface *s;

  if ((s = TTF_RenderText_Solid_Wrapped(font, text, color, wrap_length)) ==
      NULL) {
    LOG_ERROR("render_text: %s", SDL_GetError());
    return -1;
  }

  SDL_Texture *t;
  if ((t = SDL_CreateTextureFromSurface(renderer, s)) == NULL) {
    LOG_ERROR("render_text: %s", SDL_GetError());
    SDL_FreeSurface(s);
    return -1;
  }

  SDL_FreeSurface(s); // we no longer need this surface

  SDL_Rect r;
  if (SDL_QueryTexture(t, NULL, NULL, &r.w, &r.h) != 0) {
    goto error_out;
  }

  r.x = x;
  r.y = y;

  if (SDL_RenderCopy(renderer, t, NULL, &r) != 0) {
    goto error_out;
  }

  SDL_DestroyTexture(t);
  return 0;

// For the interested reader, using goto is not as taboo as it might seem for
// this particular use-case: error handling with forward jumps only. Here is a
// Linux Kernel thread by Robert Love on this:
// https://lkml.org/lkml/2003/1/12/203. Here is some Stackoverflow discussion:
// https://stackoverflow.com/questions/788903/valid-use-of-goto-for-error-management-in-c.
// We use it sparingly here.
error_out:
  LOG_ERROR("%s", SDL_GetError());
  SDL_DestroyTexture(t);
  return -1;
}

int scene_raw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                   const Uint32 wrap_length, const int x, const int y) {
  return scene_raw_text_with_color(renderer, font, text, wrap_length, x, y,
                                   FG_COLOR);
}

int scene_message(struct scene_state *s, const struct message *msg) {
  assert_not_null(4, s, msg, s->renderer, s->font);

  // first we draw the text box
  if (text_box(s) != 0) {
    // All errors are logged by text_box
    return -1;
  }

  // if the message is empty, no need to render it (otherwise SDL throws a text
  // width is zero error)
  if (strnlen(msg->m, 1) == 0) {
    return 0;
  }

  Uint32 wrap_length = TEXT_WIDTH * SIZE_FACTOR - PADDING * 2;
  int x = PADDING;
  int y = LEVEL_HEIGHT * SIZE_FACTOR + PADDING;

  if (scene_raw_text(s->renderer, s->font, msg->m, wrap_length, x, y) != 0) {
    // All errors are logged by render_text
    return -1;
  }

  return 0;
}

int scene_player_status(struct scene_state *s, const unsigned int lives,
                        const unsigned int coins) {
  assert_not_null(2, s, s->font);

  if (TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR * 0.75) != 0) {
    LOG_ERROR("%s", TTF_GetError());
    return -1;
  }

  unsigned int fps = (unsigned int)fps_get();

  const size_t BUF_LEN = 255;
  char buf[BUF_LEN];

  snprintf(buf, BUF_LEN, "     LIVES: %u        COINS: %u        FPS: %u",
           lives, coins, fps);

  int x = STATUS_PADDING;
  int y = STATUS_PADDING;

  if (scene_raw_text(s->renderer, s->font, buf, 0, x, y) != 0) {
    // All errors are logged by render_text
    if (TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR) != 0) {
      LOG_ERROR("%s", TTF_GetError());
    }

    return -1;
  }

  if (TTF_SetFontSize(s->font, DEFAULT_FONT_SIZE * SIZE_FACTOR) != 0) {
    LOG_ERROR("%s", TTF_GetError());
    return -1;
  }

  return 0;
}
