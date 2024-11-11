#include "handlers.h"

#include "fps.h"
#include "player.h"
#include "safe.h"
#include "state.h"
#include "util.h"

// the speed with which the platform moves
#define SPEED 36

int handler_platform_init(struct sprite *s) {
  assert_not_null(1, s);
  s->vx = SPEED;
  return 0;
}

int handler_platform_frame(struct sprite *s) {
  assert_not_null(1, s);

  // process horizontal movement
  enum collision c = util_move_x(s);
  if (c != COLLISION_NONE) {
    s->vx = -s->vx;
  }

  return 0;
}
int handler_platform_hit(struct sprite *s) {
  struct player *p = g_game.player;

  assert_not_null(3, p, s, p->s);

  // we adapted the platform collision logic below from the open source (MIT
  // licensed) project "sdl-platformer"
  // (https://github.com/artureganyan/sdl-platformer)

  const double dt = fps_frame_time() / 1000.0;
  struct borders tmp, pa, sa;
  int i_tmp;

  util_sprite_hints(p->s, &i_tmp, &i_tmp, &tmp, &pa);
  util_sprite_hints(s, &i_tmp, &i_tmp, &tmp, &sa);

  // if these are not present, the player jumps to the top of the platform from
  // the side of the platform.
  // TODO: try to figure out why this happens
  const double dx = 3;
  const double dy = 3;

  const SDL_Rect p_body = p->s->type->body;

  const double dh = (SPRITE_SIZE - p_body.h) / 2.0;
  const double dw = (SPRITE_SIZE - p_body.w) / 2.0;

  bool collision_x = pa.l + dx <= sa.r && pa.r - dx >= sa.l;
  bool collision_y = pa.t + dy < sa.b && pa.b - dy > sa.t;

  // this first case is slightly more complicated as we want the player to
  // move with the platform. if 1) the player's bottom is above the platform's
  // top, 2) the player's bottom is above the platform's bottom, and 3) there is
  // horizontal collision
  if (pa.b > sa.t && pa.b < sa.b && collision_x) {

    if (p->s->vx == 0) {
      p->s->x += s->vx * dt;
    }

    p->s->y = sa.t - dh - p_body.h;
    p->air = 0;
    return 0;
  }

  // if 1) the player's top is above the platform's bottom, 2) the player's top
  // is below the platform's top, and 3) there is horizontal collision
  if (pa.t < sa.b && pa.t > sa.t && collision_x) {
    p->s->y = sa.b - dh;
    return 0;
  }

  // if 1) the player's left is more left than the platform's right, 2) the
  // player's left is less left than the platform's left, and 3) there is
  // vertical collision.
  if (pa.l < sa.r && pa.l > sa.l && collision_y) {
    p->s->x = sa.r - dw;
    return 0;
  }

  // if 1) the player's right is more right than the platform's left, 2) the
  // player's right is less right than the platform's right, and 2) there is
  // vertical collision.
  if (pa.r > sa.l && pa.r < sa.r && collision_y) {
    p->s->x = sa.l - dw - p_body.w;
    return 0;
  }

  return 0;
}
