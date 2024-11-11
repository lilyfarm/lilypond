#include "handlers.h"

#include "fps.h"
#include "message.h"
#include "player.h"
#include "safe.h"
#include "state.h"
#include "util.h"

static const Uint64 INTERACTION_DELAY = 1000;

// logic for the helper character in the game

int handler_helper_init(struct sprite *s) {
  assert_not_null(1, s);
  s->animation.flip = SDL_FLIP_HORIZONTAL;

  struct fps_timer *t = fps_timer_create(INTERACTION_DELAY);
  if (t == NULL) {
    return -1;
  }

  s->data.helper.interaction_timer = t;
  return 0;
}

int handler_helper_frame(struct sprite *s) {
  assert_not_null(1, s);

  struct fps_timer *t = s->data.helper.interaction_timer;
  fps_timer_iterate(t);

  struct player *p = g_game.player;
  assert_not_null(2, p, p->s);

  int pr, r, pc, c;
  util_nearest(p->s, &pr, &pc);
  util_nearest(s, &r, &c);

  // if the player is in the same row, turn towards the direction of the player
  if (pr == r) {
    s->animation.flip = (pc <= c) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // if the player is "nearby", also call hit
    if (SDL_abs(pc - c) <= 1) {
      if (s->type->hit_handler(s) != 0) {
        return -1;
      }
    }
  }

  return 0;
}

static SDL_RendererFlip horizontal_flip(const SDL_RendererFlip f) {
  return f & SDL_FLIP_HORIZONTAL;
}

static void base_hit(struct sprite *s, const char *msg) {
  struct player *p = g_game.player;
  struct message *m = g_game.message;

  const Uint8 *k = g_prog.keys;

  assert_not_null(4, p, p->s, s, m);

  struct fps_timer *t = s->data.helper.interaction_timer;

  SDL_RendererFlip pflip = horizontal_flip(p->s->animation.flip);
  SDL_RendererFlip flip = horizontal_flip(s->animation.flip);

  // we want the player to be turning towards the helper character and pressing
  // C but not *right* after interacting with the helper character, and not when
  // the player is moving or in the air or on a ladder.
  if (pflip == flip || !k[SDL_SCANCODE_C] || !fps_timer_done(t) ||
      p->s->vx != 0 || p->air || p->ladder) {
    return;
  }

  fps_timer_reset(t);

  message_set(g_game.message, msg, true);
}

int handler_helper_hit(struct sprite *s) {
  const char *msg =
      "Hey there! Just thought I'd help you figure out how things work "
      "around here. Press Z to jump and SPACE to sprint. Go through the "
      "door (press C) to continue on to the next level! Good luck!";
  base_hit(s, msg);
  return 0;
}

int handler_helper_destroy(struct sprite *s) {
  fps_timer_destroy(&s->data.helper.interaction_timer);
  return 0;
}

int handler_cat_helper_hit(struct sprite *s) {
  const char *msg =
      "Hi there! I am Lily. The spiders ahead are scary! If they hit you, they "
      "can damage you. Jump on top of the spiders to squish them. You can use "
      "X instead of Z to short jump.";
  base_hit(s, msg);
  return 0;
}

int handler_ladder_helper_hit(struct sprite *s) {
  const char *msg = "Comparing Beethoven to Rachmaninoff is like comparing a "
                    "dragon to a dragonfly. By the way, you can climb the "
                    "ladders ahead using the Up and Down arrow keys.";

  base_hit(s, msg);
  return 0;
}

int handler_ghost_helper_hit(struct sprite *s) {
  const char *msg = "Be careful of those spooky ghosts. They throw scary blue "
                    "flaming orbs that can damage you. Jump on top of the "
                    "ghosts to squish them, but you can't squish the orbs.";
  base_hit(s, msg);
  return 0;
}

int handler_helper_last_level_hit(struct sprite *s) {
  const char *msg = "This is the last level. Good luck!";
  base_hit(s, msg);
  return 0;
}

int handler_cat_helper_last_level_hit(struct sprite *s) {
  const char *msg =
      "Thank you for playing lilypond, a completely free and open source game.";
  base_hit(s, msg);
  return 0;
}

int handler_ladder_helper_last_level_hit(struct sprite *s) {
  const char *msg =
      "Let us work for a better world together, a world free of oppression.";

  base_hit(s, msg);
  return 0;
}

int handler_ghost_helper_last_level_hit(struct sprite *s) {
  const char *msg =
      "Let us continue working for the world's children. As James Baldwin "
      "said, \"The children are always ours, every single one of them, all "
      "over the globe\". Good bye!";

  base_hit(s, msg);
  return 0;
}
