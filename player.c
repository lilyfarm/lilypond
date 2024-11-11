#include "player.h"

#include "level.h"
#include "message.h"
#include "player.h"
#include "render.h"
#include "safe.h"
#include "scene.h"
#include "sound.h"
#include "state.h"
#include "util.h"

static const Sint64 BLINK_TIME = 2000;    // milliseconds
static const Sint64 BLINK_INTERVAL = 200; // milliseconds

static const Sint64 GRAVITY = 600; // pixels per second per second
// WALK is the horizontal velocity while the player is walking, in logical
// pixels per second
static const Sint64 WALK = 72;
// SPRINT is the horizontal velocity when the player is sprinting, in logical
// pixels per second
static const Sint64 SPRINT = 96;
// LADDER is the speed when the player is climbing a ladder, in logical pixels
// per second
static const Sint64 LADDER = 64;
// FALL_MAX is the maximum speed at which a player can fall, regardless of
// gravitational acceleration. In logical pixels per second.
static const Sint64 FALL_MAX = 128;
// JUMP is the vertical velocity when the player jumps, in logical pixels per
// second.
static const Sint64 JUMP = 256;
// SHORT_JUMP is the vertical velocity when the player does a short jump, in
// logical pixels per second.
static const Sint64 SHORT_JUMP = 192;
// For animations:
static const Sint64 ANIM_WALK = 8;    // frames per second
static const Sint64 ANIM_SPRINT = 12; // frames per second
static const Sint64 ANIM_LADDER = 6;  // frames per second

// delay between toggling characters, in milliseconds
static const Uint64 CHARACTER_DELAY = 1000;

enum { CHARACTER_COUNT = 6 }; // number of choices

// cycle cycles the player sprite tile effectively "changing
// characters" in the game. index is the character to switch to
void cycle(struct player *p) {
  assert(p->index < CHARACTER_COUNT);

  size_t rows[CHARACTER_COUNT] = {1, 2, 1, 2, 2, 1};
  size_t cols[CHARACTER_COUNT] = {38, 38, 32, 26, 32, 26};

  struct sprite_type *t = p->s->type;
  sprite_type_change_tile(t, rows[p->index], cols[p->index]);

  char msg[255];
  sprintf(msg, "Switched player character.");
  message_set(g_game.message, msg, false);
}

void player_respawn_update(struct player *p) {
  assert_not_null(2, p, p->s);
  p->respawn_x = p->s->x;
  p->respawn_y = p->s->y;
}

int player_create(struct player *p) {
  assert_not_null(1, p);

  p->s = NULL; // will be created by token_player
  p->air = false;
  p->ladder = false;
  p->lives = 3;
  p->coins = 0;
  p->respawn_x = 0;
  p->respawn_y = 0;
  p->jump = true;
  p->sprint = false;
  p->index = 0;

  struct fps_timer *t = fps_timer_create(CHARACTER_DELAY);
  if (t == NULL) {
    return -1;
  }

  struct fps_timer *t2 = fps_timer_create(BLINK_TIME);
  if (t2 == NULL) {
    free(t);
    return -1;
  }

  p->character_timer = t;
  p->blink_timer = t2;

  // no blinking at the very start
  t2->time_left = 0;

  return 0;
}

static void player_respawn(struct player *p) {
  assert_not_null(2, p, p->s);

  sprite_animation_set_frame(p->s, 0, 0, 0);
  fps_timer_reset(p->blink_timer);
  p->ladder = false;
  p->air = false;
  p->s->x = p->respawn_x;
  p->s->y = p->respawn_y;
}

void player_kill(struct player *p) {
  assert_not_null(2, p, p->s);
  if (!fps_timer_done(p->blink_timer)) {
    return; // don't kill if the player is blinking
  }

  // set the animation frame to the player being dead
  sprite_animation_set_frame(p->s, 5, 5, 0);

  // play sound effect
  sound_play(SOUND_KILL, CHANNEL_PLAYER);

  if (p->lives != 0) {
    p->lives--;
  }

  if (p->lives == 0) {
    g_prog.state = PROG_GAME_OVER;
    return;
  }

  g_prog.state = PROG_GAME_KILLED;

  player_respawn(p);
}

// process_keys process how individual key presses effect the player
static void process_keys(struct player *p) {
  assert_not_null(2, p, p->s);

  struct sprite *s = p->s;
  const Uint8 *k = g_prog.keys;

  // -----------------------------
  // -- cycling the player tile --
  // -----------------------------
  if (k[SDL_SCANCODE_Q] && fps_timer_done(p->character_timer)) {
    fps_timer_reset(p->character_timer);
    p->index = (p->index + 1) % CHARACTER_COUNT;
    cycle(p);
  }

  // -------------------------
  // -- horizontal movement --
  // -------------------------
  p->sprint = k[SDL_SCANCODE_SPACE]; // sprint
  Sint64 anim_speed = p->sprint ? ANIM_SPRINT : ANIM_WALK;
  double speed = p->sprint ? SPRINT : WALK;

  if (k[SDL_SCANCODE_LEFT]) { // move left
    s->animation.flip = SDL_FLIP_HORIZONTAL;
    s->vx = -speed;

    if (!p->ladder) {
      if (p->air) {
        sprite_animation_set_frame(s, 1, 1, ANIM_WALK);
      } else {
        sprite_animation_set_frame(s, 1, 2, anim_speed);
      }
    }
  } else if (k[SDL_SCANCODE_RIGHT]) { // move right
    s->animation.flip = SDL_FLIP_NONE;
    s->vx = speed;

    if (!p->ladder) {
      if (p->air) {
        sprite_animation_set_frame(s, 1, 1, ANIM_WALK);
      } else {
        sprite_animation_set_frame(s, 1, 2, anim_speed);
      }
    }
  } else { // neither left, nor right
    s->vx = 0;
    if (p->air) {
      sprite_animation_set_frame(s, 1, 1, ANIM_WALK);
    } else if (!p->ladder) {
      sprite_animation_set_frame(s, 0, 0, 0);
    }
  }

  // -----------------------
  // -- vertical movement --
  // -----------------------

  if (k[SDL_SCANCODE_UP]) { // move up
    int r, c;
    util_nearest(s, &r, &c);

    // if the player is on a ladder
    if (util_ladder(r, c)) {
      p->ladder = true;
      s->vy = -LADDER;
      s->x = c * SPRITE_SIZE; // orient the player right on the ladder
      sprite_animation_set_flip(s, 3, ANIM_LADDER);
      p->jump = false;
    } else {
      p->ladder = false;
    }
  } else if (k[SDL_SCANCODE_DOWN]) { // move down
    int r, c;
    util_nearest(s, &r, &c);

    // if the player is either on a ladder, or right above a ladder
    if (p->ladder || util_ladder(r + 1, c)) {
      if (!p->ladder) {
        // get the player a "little bit" on the ladder
        s->y = r * SPRITE_SIZE + (SPRITE_SIZE / 2.0) + 1;
      }

      p->ladder = 1;
      s->vy = LADDER;
      s->x = c * SPRITE_SIZE; // orient the player right on the ladder
      sprite_animation_set_flip(s, 3, ANIM_LADDER);
    }
  } else { // neither up, not down
    if (p->ladder) {
      sprite_animation_set_frame(s, 3, 3, 0);
      s->vy = 0;
    } else { // we can jump again
      p->jump = true;
    }
  }

  // Jump or short jump
  if (!p->air && p->jump && (k[SDL_SCANCODE_Z] || k[SDL_SCANCODE_X])) {
    bool short_jump = k[SDL_SCANCODE_X];
    enum sound_id sound = short_jump ? SOUND_SHORT_JUMP : SOUND_JUMP;
    double speed = short_jump ? SHORT_JUMP : JUMP;

    // play the sound effect
    sound_play(sound, CHANNEL_PLAYER);

    // do the jump
    s->vy = -speed;
  }

  // Interact
  if (k[SDL_SCANCODE_C]) {
    int r, c;
    util_nearest(s, &r, &c);

    if (util_door(r, c)) {
      // play the door open sound effect
      sound_play(SOUND_DOOR, CHANNEL_SPRITE);
      g_prog.state = PROG_GAME_LEVEL_COMPLETE;
    }
  }
}

void player_iterate(struct player *p) {
  assert_not_null(2, p, p->s);
  process_keys(p);

  struct sprite *s = p->s;

  fps_timer_iterate(p->character_timer);
  fps_timer_iterate(p->blink_timer);

  // TODO: experiment with making this Uint64 and see how visible the change in
  // the gameplay is.
  const Uint64 dt_ms = fps_frame_time();
  const double dt = dt_ms / 1000.0;
  const double hit = (SPRITE_SIZE - s->type->body.w) / 2.0;

  int r, c;
  struct borders passive, actual;
  util_sprite_hints(s, &r, &c, &passive, &actual);

  s->vx = util_abs_limit(s->vx, MAX_VELOCITY);
  s->vy = util_abs_limit(s->vy, MAX_VELOCITY);

  // process horizontal movement
  s->x += s->vx * dt;
  // new borders based on the horizontal movement for collision testing.
  struct borders b = {s->x, s->x + SPRITE_SIZE, s->y, s->y + SPRITE_SIZE};

  // -- is moving left okay? --

  // if 1) our new left is more left than our old left, and 2) we are not
  // moving right.
  if (b.l < passive.l && s->vx <= 0) {
    bool left_solid = util_solid(r, c - 1, SOLID_RIGHT);
    // our top (plus some hit delta) is above the top of the passive sprite
    // "grid" location we are mapped onto and the top level passive sprite is
    // solid
    bool top_left_solid =
        b.t + hit < passive.t && util_solid(r - 1, c - 1, SOLID_RIGHT);

    // our bottom (minus some hit delta) is below the bottom of the passive
    // sprite "grid" location we are mapped onto and the bottom left passive
    // sprite is solid
    bool bottom_left_solid =
        (b.b - hit > passive.b) && util_solid(r + 1, c - 1, SOLID_RIGHT);

    if (left_solid || top_left_solid || bottom_left_solid) {
      s->x = passive.l; // put the player back onto the old "grid" location
      s->vx = 0;        // stop moving the player left
    }

    // -- is moving right okay? --
    // if 1) our new right is more right than our old right, and 2) we are
    // not moving left
  } else if (b.r > passive.r && s->vx >= 0) {
    bool right_solid = util_solid(r, c + 1, SOLID_LEFT);
    // our top (plus some hit delta) is above the top of the passive sprite
    // "grid" location we are mapped onto and the top right passive sprite is
    // solid
    bool top_right_solid =
        b.t + hit < passive.t && util_solid(r - 1, c + 1, SOLID_LEFT);
    // our bottom (minus some hit delta) is below the bottom of the passive
    // sprite "grid" location we are mapped onto and the bottom right passive
    // sprite is solid
    bool bottom_right_solid =
        b.b - hit > passive.b && util_solid(r + 1, c + 1, SOLID_LEFT);

    if (right_solid || top_right_solid || bottom_right_solid) {
      s->x = passive.l; // put the player back onto the old "grid" location
      s->vx = 0;        // stop moving the player right
    }
  }

  // process vertical movement
  s->y += s->vy * dt;
  b = (struct borders){s->x, s->x + SPRITE_SIZE, s->y, s->y + SPRITE_SIZE};

  // -- is moving down okay? --
  // if 1) our new bottom is bellow our old bottom, and 2) we are not moving up
  if (b.b > passive.b && s->vy >= 0) {
    bool bottom_solid = util_solid(r + 1, c, SOLID_TOP);

    // our left (plus some hit delta) is more left than the left of the passive
    // sprite "grid" location we are mapped onto and the bottom left passive
    // sprite is solid.
    bool bottom_left_solid =
        b.l + hit < passive.l && util_solid(r + 1, c - 1, SOLID_TOP);

    // our right (minus some hit delta) is more right than the right of the
    // passive sprite "grid" location we are mapped onto and the bottom right
    // passive sprite is solid.
    bool bottom_right_solid =
        b.r - hit > passive.r && util_solid(r + 1, c + 1, SOLID_TOP);

    bool ladder = !p->ladder && util_solid_ladder(r + 1, c);

    if (bottom_solid || bottom_left_solid || bottom_right_solid || ladder) {
      s->y = passive.t; // put the player back onto the old "grid" location
      s->vy = 0;        // stop moving the player down
      // player has touched the "ground", is no longer air-bound
      p->air = false;
      if (p->ladder) {
        // we have reached the bottom of the ladder
        p->ladder = false;
        // remove the ladder frame, returning to "walking" mode
        sprite_animation_set_frame(s, 0, 0, 0);
      }
    } else {
      // if has not touched the "ground" but the player is on a ladder, the
      // player is not air-bound. Otherwise, the player is air-bound.
      p->air = !p->ladder;
    }

    // -- is moving up okay? --
    // if 1) our new top is above our old top, and 2) we are not moving down
  } else if (b.t < passive.t && s->vy <= 0) {
    // we have moved up, if we are not on a ladder, we are air-bound.
    p->air = !p->ladder;

    // is the passive sprite "grid" location right above us solid?
    bool top_solid = util_solid(r - 1, c, SOLID_BOTTOM);

    // our left (plus some hit delta) is more left than the left of the
    // passive sprite "grid" location we are mapped onto and the top left
    // passive sprite is solid.
    bool top_left_solid =
        b.l + hit < passive.l && util_solid(r - 1, c - 1, SOLID_BOTTOM);

    // our right (minus some hit delta) is more right than the right of the
    // passive sprite "grid" location we are mapped onto and the top right
    // passive sprite is solid.
    bool top_right_solid =
        b.r - hit > passive.r && util_solid(r - 1, c + 1, SOLID_BOTTOM);

    if (top_solid || top_left_solid || top_right_solid) {
      s->y = passive.t; // put the player back onto the old "grid" location
      s->vy += 1;       // accelerate the player downwards
    }
  }

  // -- other movement --
  util_nearest(s, &r, &c); // update our passive sprite "grid" location

  // implement 2D playformer "gravity"
  if (!p->ladder) {
    s->vy += GRAVITY * dt;
    s->vy = SDL_min(s->vy, FALL_MAX);
  }

  // if we were on a ladder, but no longer are on a ladder
  if (p->ladder && !util_ladder(r, c)) {
    p->ladder = false;
    // remove the ladder frame, returning to "walking" mode
    sprite_animation_set_frame(s, 0, 0, 0);

    // if we were climbing up on the ladder
    if (s->vy < 0) {
      s->vy = 0;              // stop going up
      s->y = SPRITE_SIZE * r; // snap back onto the "grid" location
    }
  }

  // blink
  if (fps_timer_done(p->blink_timer)) {
    return;
  }

  Uint64 blink = p->blink_timer->time_left;
  p->s->animation.alpha = 255 * (1 - (blink / BLINK_INTERVAL) % 2);
}

void player_destroy(struct player **pp) {
  assert_not_null(1, pp);
  struct player *p = *pp;
  assert_not_null(1, p);

  fps_timer_destroy(&p->blink_timer);
  fps_timer_destroy(&p->character_timer);

  free(p);
  *pp = NULL;
}
