#ifndef SPRITE_TYPE_H
#define SPRITE_TYPE_H

#include <SDL2/SDL.h>

// sprite_id contains the types of sprites we have in the game
// Note that these are all sorted in descending order of rendering depth. The
// higher the depth of the sprite type, the earlier it is rendered. As an
// example, we want a player to be rendered *after* the background so the player
// is not *behind* the background. Therefore the background will have a *higher*
// depth than the player.
enum sprite_id {
  SPRITE_NONE = 0,
  // The following two elements are used for testing (e.g in array_test.c)
  SPRITE_TEST_LO_DEPTH,
  SPRITE_TEST_HI_DEPTH,

  // Passive sprites -- will always be rendered at the same depth, which is a
  // higher depth than active sprites, regardless of their position in this
  // enum.
  SPRITE_WALL_TOP,
  SPRITE_WALL,
  SPRITE_RED_WALL_TOP,
  SPRITE_RED_WALL,
  SPRITE_RUST_WALL_TOP,
  SPRITE_RUST_WALL,
  SPRITE_GROUND_TOP,
  SPRITE_GROUND,
  SPRITE_CAVE_TOP,
  SPRITE_CAVE,
  SPRITE_GRASS,
  SPRITE_LADDER,
  SPRITE_DOOR,
  SPRITE_WATER,
  SPRITE_LEFT_ARROW,

  // Active sprites -- here the depth matters
  SPRITE_WATER_TOP,
  SPRITE_PLAYER,
  SPRITE_SPIDER,
  SPRITE_SPRINTING_SPIDER,
  SPRITE_SKELETON,
  SPRITE_SHOT,
  SPRITE_GHOST,
  SPRITE_COIN,
  SPRITE_EXTRA_LIFE,
  SPRITE_HELPER,
  SPRITE_CAT_HELPER,
  SPRITE_LADDER_HELPER,
  SPRITE_GHOST_HELPER,
  SPRITE_PLATFORM,
  SPRITE_BAT,
  SPRITE_SPRING,
  SPRITE_HELPER_LAST_LEVEL,
  SPRITE_CAT_HELPER_LAST_LEVEL,
  SPRITE_LADDER_HELPER_LAST_LEVEL,
  SPRITE_GHOST_HELPER_LAST_LEVEL,
  // SPRITE_TYPE_COUNT should always be the final element as it is the size used
  // to initialize the g_sprite_types array. This is not to be confused with
  // SPRITE_COUNT defined above.
  SPRITE_TYPE_COUNT,
};

// We need to forward-declare sprite here to declare sprite_handler.
struct sprite;

// sprite_handler is used to perform functions on a particular type of sprite.
// Returns 0 on success, -1 on failure.
typedef int (*sprite_handler)(struct sprite *);

// solid_type is used for collision detection in the game logic
enum solid_type {
  SOLID_NONE = 0,
  SOLID_LEFT = (1 << 0),
  SOLID_RIGHT = (1 << 1),
  SOLID_TOP = (1 << 2),
  SOLID_BOTTOM = (1 << 3),
  SOLID_ALL = SOLID_LEFT | SOLID_RIGHT | SOLID_TOP | SOLID_BOTTOM
};

// sprite_type contains information on how to process a particular type of
// sprites.
struct sprite_type {
  // the identifier for this sprite type
  enum sprite_id id;
  // the identifier for the parent of this sprite type, if any
  enum sprite_id parent_id;
  // this is the rect of the sprite in our spritesheet, without scaling.
  SDL_Rect rect;
  // this is the rect of the body of the sprite, without scaling. As an example,
  // let's say we have a "block" sprite. Let's say in the sprite sheet, it is
  // the third sprite from the left and each sprite's width in the sprite sheet
  // is 16. Let's also assume that the block itself starts after the second
  // pixel from the left and the rest is empty space. Then rect.x would be 16 *
  // 2 and body.x would be 2.
  SDL_Rect body;
  // what type of solid are we working with?
  enum solid_type solid_type;
  // -- Now for the sprite handlers --
  // To be called when the sprite is initialized
  sprite_handler init_handler;
  // To be called at each frame
  sprite_handler frame_handler;
  // To be called on a collision
  sprite_handler hit_handler;
  // To be called when the sprite is destroyed
  sprite_handler destroy_handler;
};

// sprite_type_change_tile changes the tile used by the sprite type in the
// sprite sheet to the tile on row r and column c.
void sprite_type_change_tile(struct sprite_type *t, const size_t r,
                             const size_t c);

// g_sprite_types_create initializes all the sprite types in the g_sprite_types
// array.
int g_sprite_types_create(void);

// g_sprite_types_destroy frees all the memory allocated to sprite_types within
// the g_sprite_types array
void g_sprite_types_destroy(void);

#endif // SPRITE_TYPE_H
