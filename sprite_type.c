#include "sprite_type.h"

#include "base.h"
#include "handlers.h"
#include "safe.h"
#include "state.h"

void sprite_type_change_tile(struct sprite_type *t, const size_t r,
                             const size_t c) {
  assert_not_null(1, t);
  t->rect.y = r * SPRITE_SIZE;
  t->rect.x = c * SPRITE_SIZE;
}

// sprite_type_init is a helper function to initialize a sprite_type in the
// g_sprite_types array. Returns 0 on success, -1 on error.
static void sprite_type_init(
    const enum sprite_id id, const enum solid_type solid_type, const int row,
    const int column, const int width, int height, const SDL_Rect body,
    const sprite_handler init_handler, const sprite_handler frame_handler,
    const sprite_handler hit_handler, const sprite_handler destroy_handler) {

  struct sprite_type *type = g_sprite_types[id];

  type->id = id;
  type->parent_id = id;

  // the co-ordinated and dimensions of the tile the sprite is in on the sprite
  // sheet.
  type->rect.y = row * SPRITE_SIZE;
  type->rect.x = column * SPRITE_SIZE;
  type->rect.w = width;
  type->rect.h = height;

  // the offset of the *actual* sprite illustration from the tile the sprite is
  // in on the sprite sheet.
  // As an example, say we have an 8x8 block sprite:
  // --------
  // --****--
  // --****--
  // --****--
  // --****--
  // --****--
  // --****--
  // --------
  // Where the * character represents the actual illustration and the -
  // character represents the color we mark as transparent.
  // Then the body of the sprite would be
  // SDL_Rect body;
  // body.x = 2; // 2 pixels from the left
  // body.w = 4; // the illustration is 4 pixels wide
  // body.y = 1; // 1 pixel from the top
  // body.h = 6; // the illustration is 6 pixels high
  type->body = body;

  type->solid_type = solid_type;

  // The sprite handlers
  type->init_handler = init_handler;
  type->frame_handler = frame_handler;
  type->hit_handler = hit_handler;
  type->destroy_handler = destroy_handler;
}

// sprite_type_init_d (the "d" stands for default) is a helper function to
// initialize a sprite type in the g_sprite_types array, with certain default
// values. Returns 0 on success, -1 on error.
static void sprite_type_init_d(const enum sprite_id id,
                               const enum solid_type solid_type, const int row,
                               const int column) {
  sprite_type_init(id, solid_type, row, column, SPRITE_SIZE, SPRITE_SIZE,
                   (SDL_Rect){0, 0, SPRITE_SIZE, SPRITE_SIZE},
                   handler_sprite_init, handler_sprite_frame,
                   handler_sprite_hit, handler_sprite_destroy);
}

// ----------------------------------
// -- Initialize every sprite type --
// ----------------------------------

int g_sprite_types_create(void) {
  register size_t i;

  for (i = 0; i < SPRITE_TYPE_COUNT; i++) {
    g_sprite_types[i] = calloc(1, sizeof(struct sprite_type));
    if (g_sprite_types[i] == NULL) {
      g_sprite_types_destroy();
      return -1;
    }
  }

  // we pick a transparent block in the sprite sheet and mark it as SOLID_NONE
  // to represent SPRITE_NONE.
  sprite_type_init_d(SPRITE_NONE, SOLID_NONE, 0, 10);

  // top of the wall
  sprite_type_init_d(SPRITE_WALL_TOP, SOLID_ALL, 4, 2);

  // inside of the wall
  sprite_type_init_d(SPRITE_WALL, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT, 5,
                     2);

  // top of the rust wall
  sprite_type_init_d(SPRITE_RUST_WALL_TOP, SOLID_ALL, 4, 3);

  // inside of the rust wall
  sprite_type_init_d(SPRITE_RUST_WALL, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT,
                     5, 3);

  // top of the red wall
  sprite_type_init_d(SPRITE_RED_WALL_TOP, SOLID_ALL, 4, 5);

  // inside of the red wall
  sprite_type_init_d(SPRITE_RED_WALL, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT,
                     5, 5);

  // top of the ground
  sprite_type_init_d(SPRITE_GROUND_TOP, SOLID_ALL, 6, 1);

  // inside of the ground
  sprite_type_init_d(SPRITE_GROUND, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT, 7,
                     1);

  // top of the cave
  sprite_type_init_d(SPRITE_CAVE_TOP, SOLID_ALL, 6, 0);

  // inside of the cave
  sprite_type_init_d(SPRITE_CAVE, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT, 7,
                     0);

  // grass
  sprite_type_init_d(SPRITE_GRASS, SOLID_NONE, 40, 1);

  // inside of the ground
  sprite_type_init_d(SPRITE_WALL, SOLID_RIGHT | SOLID_BOTTOM | SOLID_LEFT, 5,
                     2);

  // the ladder
  sprite_type_init_d(SPRITE_LADDER, SOLID_NONE, 12, 2);

  // the player
  sprite_type_init(SPRITE_PLAYER, SOLID_ALL, 1, 38, 16, 16,
                   (SDL_Rect){6, 0, 4, 16}, handler_sprite_init,
                   handler_sprite_frame, handler_sprite_hit,
                   handler_sprite_destroy);

  // coins
  sprite_type_init(SPRITE_COIN, SOLID_ALL, 47, 29, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_item_init,
                   handler_item_frame, handler_item_hit,
                   handler_sprite_destroy);

  // extra life
  sprite_type_init(SPRITE_EXTRA_LIFE, SOLID_ALL, 55, 27, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_item_init,
                   handler_item_frame, handler_item_hit,
                   handler_sprite_destroy);

  // spider
  sprite_type_init(SPRITE_SPIDER, SOLID_ALL, 11, 38, 16, 16,
                   (SDL_Rect){0, 6, 15, 10}, handler_spider_init,
                   handler_spider_frame, handler_spider_hit,
                   handler_sprite_destroy);

  // bat
  sprite_type_init(SPRITE_BAT, SOLID_ALL, 8, 26, 16, 16,
                   (SDL_Rect){0, 3, 16, 10}, handler_bat_init,
                   handler_spider_frame, handler_spider_hit,
                   handler_sprite_destroy);

  // sprinting spider
  sprite_type_init(SPRITE_SPRINTING_SPIDER, SOLID_ALL, 11, 32, 16, 16,
                   (SDL_Rect){0, 6, 15, 10}, handler_spider_init,
                   handler_sprinting_spider_frame, handler_spider_hit,
                   handler_sprite_destroy);

  // skeleton
  sprite_type_init(SPRITE_SKELETON, SOLID_ALL, 6, 26, 16, 16,
                   (SDL_Rect){0, 6, 15, 10}, handler_skeleton_init,
                   handler_skeleton_frame, handler_spider_hit,
                   handler_sprite_destroy);

  // ghost
  sprite_type_init(SPRITE_GHOST, SOLID_ALL, 7, 26, 16, 16,
                   (SDL_Rect){0, 6, 15, 10}, handler_ghost_init,
                   handler_ghost_frame, handler_spider_hit,
                   handler_sprite_destroy);

  // shot
  sprite_type_init(SPRITE_SHOT, SOLID_ALL, 52, 0, 16, 16,
                   (SDL_Rect){0, 4, 16, 7}, handler_shot_init,
                   handler_shot_frame, handler_shot_hit,
                   handler_sprite_destroy);

  // water top
  sprite_type_init(SPRITE_WATER_TOP, SOLID_NONE, 54, 0, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_water_init,
                   handler_sprite_frame, handler_water_hit,
                   handler_sprite_destroy);

  // water
  sprite_type_init_d(SPRITE_WATER, SOLID_NONE, 55, 0);

  // door -- typically leads to the next level
  sprite_type_init_d(SPRITE_DOOR, SOLID_NONE, 10, 0);

  // helper character
  sprite_type_init(SPRITE_HELPER, SOLID_ALL, 56, 0, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_helper_hit,
                   handler_helper_destroy);

  // cat helper character
  sprite_type_init(SPRITE_CAT_HELPER, SOLID_ALL, 56, 1, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_cat_helper_hit,
                   handler_helper_destroy);

  // ladder helper character
  sprite_type_init(SPRITE_LADDER_HELPER, SOLID_ALL, 56, 2, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_ladder_helper_hit,
                   handler_helper_destroy);

  // ghost helper character
  sprite_type_init(SPRITE_GHOST_HELPER, SOLID_ALL, 56, 3, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_ghost_helper_hit,
                   handler_helper_destroy);

  // left arrow
  sprite_type_init_d(SPRITE_LEFT_ARROW, SOLID_NONE, 51, 2);

  // platform
  sprite_type_init(SPRITE_PLATFORM, SOLID_NONE, 23, 3, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_platform_init,
                   handler_platform_frame, handler_platform_hit,
                   handler_sprite_destroy);

  // spring
  sprite_type_init(SPRITE_SPRING, SOLID_NONE, 20, 2, 16, 16,
                   (SDL_Rect){0, 8, 16, 8}, handler_spring_init,
                   handler_spring_frame, handler_spring_hit,
                   handler_spring_destroy);

  // last level helpers
  // helper character
  sprite_type_init(SPRITE_HELPER_LAST_LEVEL, SOLID_ALL, 56, 0, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_helper_last_level_hit,
                   handler_helper_destroy);

  // cat helper character
  sprite_type_init(SPRITE_CAT_HELPER_LAST_LEVEL, SOLID_ALL, 56, 1, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_cat_helper_last_level_hit,
                   handler_helper_destroy);

  // ladder helper character
  sprite_type_init(SPRITE_LADDER_HELPER_LAST_LEVEL, SOLID_ALL, 56, 2, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_ladder_helper_last_level_hit,
                   handler_helper_destroy);

  // ghost helper character
  sprite_type_init(SPRITE_GHOST_HELPER_LAST_LEVEL, SOLID_ALL, 56, 3, 16, 16,
                   (SDL_Rect){0, 0, 16, 16}, handler_helper_init,
                   handler_helper_frame, handler_ghost_helper_last_level_hit,
                   handler_helper_destroy);

  return 0;
}

void g_sprite_types_destroy(void) {
  register size_t i;
  for (i = 0; i < SPRITE_TYPE_COUNT; i++) {
    if (g_sprite_types[i] != NULL) {
      free(g_sprite_types[i]);
      g_sprite_types[i] = NULL;
    }
  }
}
