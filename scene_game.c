#include "scene.h"

#include "camera.h"
#include "default_levels.h"
#include "fps.h"
#include "level.h"
#include "message.h"
#include "player.h"
#include "safe.h"
#include "state.h"

// DEBUG_LEVEL_START is the level to start the game with. Can be changed to a
// value other than 0, for debugging specific levels.
static const size_t DEBUG_LEVEL_START = 0;

// This file contains the implementation for SCENE_GAME. Any functions required
// outside the scope of this file are declared in scene.h.

static size_t _level;

// secret_timer is a timer for a secret cheat code in the game that makes the
// player scroll through levels
static struct fps_timer *secret_timer = NULL;

static const Uint64 SECRET_DELAY = 250;

// multiplies the size factor to a rect
static void rect_factor_size(SDL_Rect *r) {
  r->x *= SIZE_FACTOR;
  r->y *= SIZE_FACTOR;
  r->w *= SIZE_FACTOR;
  r->h *= SIZE_FACTOR;
}

// render all active sprites
static int active_sprites(struct scene_state *state, struct level *l,
                          SDL_Rect *cam) {
  struct array *arr = l->active_sprites;
  SDL_Texture *sheet = state->sprite_sheet;

  assert_not_null(5, state, l, cam, arr, sheet);

  const double dt = fps_frame_time(); // in milliseconds

  register size_t i;
  for (i = 0; i < arr->l; i++) {
    struct sprite *s = arr->a[i];
    assert_not_null(1, s);

    if (s->removed) {
      // don't render removed sprites
      continue;
    }

    // -- animation --
    struct animation *a = &s->animation;
    a->frame_delay_counter -= dt;
    if (a->frame_delay_counter <= 0) {
      a->frame_delay_counter = a->frame_delay; // reset
      a->frame += 1;                           // increment frame
      if (a->frame > a->frame_end) {
        a->frame = a->frame_start; // loop frames
      }

      if (a->type == ANIMATION_FLIP) { // switch out the flip
        if (a->flip == SDL_FLIP_HORIZONTAL) {
          a->flip = SDL_FLIP_NONE;
        } else {
          a->flip = SDL_FLIP_HORIZONTAL;
        }
      }
    }

    SDL_SetTextureAlphaMod(sheet, a->alpha);

    // -- render the active sprite --
    SDL_Rect src = s->type->rect;
    if (a->type == ANIMATION_FRAME_VERTICAL) {
      src.y += src.h * a->frame;
    } else {
      src.x += src.w * a->frame;
    }

    SDL_Rect dst = {
        s->x,  // x
        s->y,  // y
        src.w, // w
        src.h  // h
    };

    if (!camera_map(cam, &dst)) {
      continue;
    }

    rect_factor_size(&dst);

    if (SDL_RenderCopyEx(state->renderer, sheet, &src, &dst, 0, NULL,
                         a->flip) != 0) {
      LOG_ERROR("%s", SDL_GetError());
      return -1;
    }

    SDL_SetTextureAlphaMod(sheet, 255);
  }

  return 0;
}

// render all passive sprites
static int passive_sprites(struct scene_state *s, struct level *l,
                           SDL_Rect *cam) {
  register size_t r, c;
  for (r = 0; r < ROW_COUNT * l->h; r++) {
    for (c = 0; c < COLUMN_COUNT * l->w; c++) {

      struct sprite_type *t = l->passive_sprites[r * COLUMN_COUNT * l->w + c];

      // draw the passive sprite
      SDL_Rect src = t->rect; // source rect
      // destination rect
      SDL_Rect dst = {SPRITE_SIZE * c, SPRITE_SIZE * r, src.w, src.h};
      if (!camera_map(cam, &dst)) {
        continue;
      }

      rect_factor_size(&dst);

      if (SDL_RenderCopy(s->renderer, s->sprite_sheet, &src, &dst) != 0) {
        LOG_ERROR("%s", SDL_GetError());
        return -1;
      }
    }
  }

  return 0;
}

// g_game_create initializes the global game state. Returns 0 on success, -1 on
// failure.
static int g_game_create(void) {
  // start at the first level, unless we are debugging.
  _level = DEBUG_LEVEL_START;
  g_prog.state = PROG_GAME_IN; // we are officially in the game
  g_game.level = NULL;
  g_game.player = malloc(sizeof(struct player));
  if (g_game.player == NULL) {
    // errno = ENOMEM;
    return -1;
  }

  // start with an empty, null-terminated message
  g_game.message = message_create();
  if (g_game.message == NULL) {
    return -1;
  }

  // initialize the player
  if (player_create(g_game.player) != 0) {
    free(g_game.player);
    g_game.player = NULL;
    return -1;
  }

  return 0;
}

// g_game_destroy destroys all game state and frees up any memory allocated on
// the heap in the g_game struct.
static void g_game_destroy(void) {
  // If we currently have a level, free that level
  struct level *l = g_game.level;
  if (l != NULL) {
    level_free(&l); // this should also free player->sprite
  }

  message_destroy(&g_game.message);

  // Free the player
  player_destroy(&g_game.player);

  // free the sprite types
  g_sprite_types_destroy();

  // if we have a custom level path set, free it.
  if (g_game.custom_level_path != NULL) {
    free(g_game.custom_level_path);
    g_game.custom_level_path = NULL;
  }
}

static int load_current_level(void) {
  const char *filename;
  const struct token_entry *tokens;
  size_t tokens_count;

  bool custom = g_game.custom_level_path != NULL;
  if (custom) {
    filename = g_game.custom_level_path;
    tokens = CUSTOM_LEVEL_TOKENS;
    tokens_count = CUSTOM_LEVEL_TOKEN_COUNT;
  } else {
    filename = LEVELS[_level];
    tokens = LEVEL_TOKENS[_level];
    tokens_count = LEVEL_TOKENS_COUNT[_level];
  }

  if (level_load(filename, tokens, tokens_count) != 0) {
    LOG_ERROR("failed to load %s", filename);
    return -1;
  }

  LOG_INFO("level %lu loaded.", (unsigned long)_level);
  return 0;
}

static int scene_game_create(void) {
  secret_timer = fps_timer_create(SECRET_DELAY);
  if (secret_timer == NULL) {
    return -1;
  }

  // initialize the sprite types
  if (g_sprite_types_create() != 0) {
    LOG_ERROR("failed to load sprite types");
  }

  // initialize the global game state
  if (g_game_create() != 0) {
    LOG_ERROR("failed to load game state");
    return -1;
  }

  if (load_current_level() != 0) {
    return -1;
  }

  struct level *l = g_game.level;
  camera_create(&g_game.camera, g_game.player->s, l->w, l->h);

  if (g_game.custom_level_path == NULL) {
    message_set(
        g_game.message,
        "Press C to interact with people and to dismiss this message. Use the "
        "LEFT and RIGHT arrow keys to walk. Press Q to toggle character.",
        false);
    return 0;
  }

  message_set(g_game.message, "Loaded user-created level.", false);
  return 0;
}

static int scene_game_destroy(void) {
  assert_not_null(1, secret_timer);
  fps_timer_destroy(&secret_timer);
  // free up any elements that are taking up heap memory in g_game
  g_game_destroy();
  return 0;
}

static bool _game_over_message_set = false;

static int scene_game_iterate(void) {
  const Uint8 *k = g_prog.keys;
  enum prog_state *s = &g_prog.state;
  struct message *msg = g_game.message;

  if (*s == PROG_GAME_LEVEL_COMPLETE) {
    _level = SDL_min(_level + 1, LEVEL_COUNT);

    if (_level == LEVEL_COUNT) {
      *s = PROG_GAME_COMPLETE;
      scene_change(SCENE_END);
      return 0;
    }

    *s = PROG_GAME_IN;

    if (load_current_level() != 0) {
      return -1;
    }

    assert(g_game.player->s != NULL);
  }

  if (k[SDL_SCANCODE_ESCAPE]) {
    return scene_change(SCENE_MENU);
  }

  if (g_prog.state == PROG_GAME_OVER) {

    if (!_game_over_message_set) {
      message_set(msg, "Game over. Press ESC to return to menu.", false);
      _game_over_message_set = true;
    }

    return 0;
  }

  _game_over_message_set = false;

  message_iterate(msg, k[SDL_SCANCODE_C]);

  if (message_block(msg)) {
    return 0;
  }

  player_iterate(g_game.player);

  if (level_iterate(g_game.level) != 0) {
    return -1;
  }

  // secret code to toggle levels
  fps_timer_iterate(secret_timer);

  if (!fps_timer_done(secret_timer)) {
    return 0;
  }

  // Ctrl + Shift + Tab + 0
  if (k[SDL_SCANCODE_LCTRL] && k[SDL_SCANCODE_LSHIFT] &&
      k[SDL_SCANCODE_SPACE] && k[SDL_SCANCODE_0]) {
    fps_timer_reset(secret_timer);
    *s = PROG_GAME_LEVEL_COMPLETE;
  }

  return 0;
}

static int scene_game_render(struct scene_state *s) {
  struct player *p = g_game.player;
  struct level *l = g_game.level;
  struct message *msg = g_game.message;

  SDL_Rect *cam = &g_game.camera;
  assert_not_null(4, s, p, p->s, l);

  // update the camera based on the player's current location
  camera_update(cam, p->s, l->w, l->h);

  // render all the passive sprites
  passive_sprites(s, l, cam);

  // render all active sprites
  active_sprites(s, l, cam);

  // the status and message are deliberately render *after* the sprites so they
  // aren't "behind" the sprites

  // ls render the game status
  if (scene_player_status(s, p->lives, p->coins) != 0) {
    return -1;
  }

  // render the game message
  if (scene_message(s, msg) != 0) {
    return -1;
  }

  return 0;
}

void scene_game_load(void) {
  struct scene *s = g_scenes[SCENE_GAME];
  s->create = scene_game_create;
  s->destroy = scene_game_destroy;
  s->iterate = scene_game_iterate;
  s->render = scene_game_render;
}
