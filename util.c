#include "util.h"

#include "base.h"
#include "fps.h"
#include "game.h"
#include "level.h"
#include "safe.h"
#include "state.h"

#include <errno.h>
#include <limits.h>

bool util_fair_coin_flip(void) { return rand() % 2 == 0; }

static bool valid_x(const int c) {
  struct level *l = g_game.level;
  assert_not_null(1, l);
  return (c >= 0 && c < COLUMN_COUNT * size_t_to_int(l->w));
}

static bool valid_y(const int r) {
  struct level *l = g_game.level;
  assert_not_null(1, l);
  return (r >= 0 && r < ROW_COUNT * size_t_to_int(l->h));
}

// valid checks if the row and column is within the screen
static bool valid(const int r, const int c) {
  struct level *l = g_game.level;
  assert_not_null(1, l);
  return (r >= 0 && r < ROW_COUNT * size_t_to_int(l->h) && c >= 0 &&
          c < COLUMN_COUNT * size_t_to_int(l->w));
}

void util_nearest(const struct sprite *s, int *r, int *c) {
  assert_not_null(3, s, r, c);
  const SDL_Rect b = s->type->body;
  // since in the C programming language, integer division always rounds towards
  // zero, our earlier code here, for example: *r = (s->y + b.y + b.h / 2.0) /
  // SPRITE_SIZE; was causing a subtle bug, where if (s->y + b.y + b.h / 2.0) /
  // SPRITE_SIZE is -0.7, say, it would round to 0, instead of -1. This causes
  // an issue with the game physics in the rare case where the player is above
  // the screen. To fix this, we will use the more explicit floor function
  // instead.
  *r = SDL_floor((s->y + b.y + b.h / 2.0) / SPRITE_SIZE);
  *c = SDL_floor((s->x + b.x + b.w / 2.0) / SPRITE_SIZE);
}

bool util_ladder(const int r, const int c) {
  struct level *l = g_game.level;
  assert_not_null(1, l);

  if (!valid(r, c)) {
    return false;
  }

  struct sprite_type *t = l->passive_sprites[r * COLUMN_COUNT * l->w + c];

  return t->parent_id == SPRITE_LADDER;
}

bool util_door(const int r, const int c) {
  struct level *l = g_game.level;
  assert_not_null(1, l);

  if (!valid(r, c)) {
    return false;
  }

  struct sprite_type *t = l->passive_sprites[r * COLUMN_COUNT * l->w + c];

  return t->parent_id == SPRITE_DOOR;
}

// util_borders takes a sprite and populates a borders struct corresponding to
// the sprite.
static void util_borders(const struct sprite *s, struct borders *b) {
  assert_not_null(2, s, b);
  const SDL_Rect r = s->type->body;
  b->l = s->x + r.x;
  b->r = b->l + r.w;
  b->t = s->y + r.y;
  b->b = b->t + r.h;
}

bool util_collide(const struct sprite *s1, const struct sprite *s2) {
  // Calculate the edges of each sprite's bounding box
  struct borders b1, b2;
  util_borders(s1, &b1);
  util_borders(s2, &b2);

  // Check for overlap on both axes
  return (b1.l < b2.r && b1.r > b2.l && b1.t < b2.b && b1.b > b2.t);
}

void util_sprite_hints(const struct sprite *s, int *r, int *c,
                       struct borders *passive, struct borders *actual) {
  util_nearest(s, r, c);
  util_borders(s, actual);

  passive->l = SPRITE_SIZE * (*c);
  passive->r = passive->l + SPRITE_SIZE;
  passive->t = SPRITE_SIZE * (*r);
  passive->b = passive->t + SPRITE_SIZE;
}

double util_abs_limit(const double v, const double m) {
  if (v > m) {
    return m;
  }

  if (v < -m) {
    return -m;
  }

  return v;
}

bool util_solid(const int r, const int c, const enum solid_type s) {
  if (!valid_x(c)) {
    return true;
  }

  if (!valid_y(r)) {
    return false;
  }

  assert_not_null(1, g_game.level);
  struct level *l = g_game.level;
  return (l->passive_sprites[r * COLUMN_COUNT * l->w + c]->solid_type & s) == s;
}

bool util_solid_ladder(const int r, const int c) {
  if (!util_ladder(r, c)) {
    return false;
  }

  return (util_solid(r, c - 1, SOLID_TOP) || util_solid(r, c + 1, SOLID_TOP) ||
          !util_ladder(r - 1, c));
}

bool util_visible(const struct sprite *s1, const struct sprite *s2) {
  struct level *l = g_game.level;
  assert_not_null(3, s1, s2, l);

  // we can assume here that both s1 and s2 are on the screen, as this function
  // is intended to be called from within a sprite's frame or hit handlers which
  // should only be called by a scene (e.g scene_game.c) if a sprite is on the
  // screen.

  int r1, c1, r2, c2;
  util_nearest(s1, &r1, &c1);
  util_nearest(s2, &r2, &c2);

  // if we are not in the same row, return false
  if (r1 != r2) {
    return false;
  }

  // which direction is s1 looking at
  enum direction dir;
  dir = s1->animation.flip == SDL_FLIP_NONE ? DIR_RIGHT : DIR_LEFT;

  register int i;

  if (dir == DIR_LEFT) {
    if (c1 <= c2) {
      return false;
    }

    // s2 is to the left of s1, but are there any solid passive sprites between
    // these two?
    for (i = c2; i < c1; i++) {
      if (util_solid(r1, i, SOLID_LEFT)) {
        return false;
      }
    }

    return true;
  }

  // DIR_RIGHT
  if (c1 >= c2) {
    return false;
  }

  for (i = c1; i < c2; i++) {
    if (util_solid(r1, i, SOLID_RIGHT)) {
      return false;
    }
  }

  return true;
}

long util_level_from_file(const char *filename, char **str, size_t *w,
                          size_t *h) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    LOG_ERROR("error opening file: %s", filename);
    return -1;
  }

  // get file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  // allocate memory for the string
  // TODO: find a safer way to convert this long to size_t
  char *content = malloc((size_t)(file_size + 1));
  if (content == NULL) {
    perror("memory allocation failed");
    fclose(file);
    return -1;
  }

  // read file content
  size_t total_read = 0;
  size_t lines = 0;

  char ch;
  bool first_line = true;
  while ((ch = fgetc(file)) != EOF) {
    if (ch == '\n' && first_line) {
      lines++;
      first_line = false;

      if (total_read % COLUMN_COUNT != 0) {
        LOG_ERROR("incorrect level width for %s, got width %lu (expected "
                  "multiple of %u)",
                  filename, (unsigned long)total_read, COLUMN_COUNT);
        goto error_out;
      }

      *w = total_read / COLUMN_COUNT;
      continue;
    }

    if (ch == '\n') {
      lines++;
      continue;
    }

    content[total_read] = ch;
    total_read++;
  }

  if (first_line) {
    LOG_ERROR("first line of level %s not found", filename);
    goto error_out;
  }

  // last line does not end with a new line
  lines++;

  if (lines % ROW_COUNT != 0) {
    LOG_ERROR("incorrect level height for %s, got height %lu (expected "
              "multiple of %u), last "
              "line should *NOT* end with a new line character.",
              filename, (unsigned long)lines, ROW_COUNT);
    goto error_out;
  }

  *h = lines / ROW_COUNT;

  content[total_read] = '\0'; // null-terminate the string

  fclose(file);
  *str = content;

  LOG_INFO_VERBOSE("loaded level file %s [w = %lu, h = %lu]", filename,
                   (unsigned long)*w, (unsigned long)*h);

  return file_size;

error_out:
  errno = EINVAL;
  fclose(file);
  free(content);
  return -1;
}

enum collision util_move_x(struct sprite *s) {
  int r, c;
  struct borders passive, actual;
  util_sprite_hints(s, &r, &c, &passive, &actual);

  const double dt = fps_frame_time() / 1000.0;
  s->vx = util_abs_limit(s->vx, MAX_VELOCITY);
  s->x += s->vx * dt;

  // new borders based on the horizontal movement for collision testing.
  struct borders b = {s->x, s->x + SPRITE_SIZE, s->y, s->y + SPRITE_SIZE};

  // if 1) our new left is more left than our old left, and 2) we are not
  // moving right.
  if (b.l < passive.l && s->vx <= 0) {
    bool left_solid = util_solid(r, c - 1, SOLID_RIGHT);

    if (left_solid) {
      s->x = passive.l; // put the sprite back onto the old "grid" location
      return COLLISION_LEFT;
    }

    return COLLISION_NONE;
  }

  // -- is moving right okay? --
  // if 1) our new right is more right than our old right, and 2) we are
  // not moving left
  if (b.r > passive.r && s->vx >= 0) {
    bool right_solid = util_solid(r, c + 1, SOLID_LEFT);

    if (right_solid) {
      s->x = passive.l; // put the player back onto the old "grid" location
      return COLLISION_RIGHT;
    }
  }

  return COLLISION_NONE;
}
