#ifndef HANDLERS_H
#define HANDLERS_H

// Contains all the sprite handlers for the various sprite types in the game
#include "sprite.h"

// the handlers for the simplest kind of sprite
int handler_sprite_init(struct sprite *s);
int handler_sprite_frame(struct sprite *s);
int handler_sprite_hit(struct sprite *s);
int handler_sprite_destroy(struct sprite *s);

// the handlers for item collection
int handler_item_init(struct sprite *s);
int handler_item_frame(struct sprite *s);
int handler_item_hit(struct sprite *s);

// the handler for spiders and similar enemies
int handler_spider_init(struct sprite *s);
int handler_spider_frame(struct sprite *s);
int handler_spider_hit(struct sprite *s);

int handler_sprinting_spider_frame(struct sprite *s);
int handler_bat_init(struct sprite *s);

// the handler for ghosts
int handler_ghost_init(struct sprite *s);
int handler_ghost_frame(struct sprite *s);

// the handler for things shot at the player by enemies
int handler_shot_init(struct sprite *s);
int handler_shot_frame(struct sprite *s);
int handler_shot_hit(struct sprite *s);

// water uses the same frame handler as the simplest kind of sprite.
int handler_water_init(struct sprite *s);
int handler_water_hit(struct sprite *s);

// the helper characters
int handler_helper_init(struct sprite *s);
int handler_helper_frame(struct sprite *s);
int handler_helper_hit(struct sprite *s);
int handler_helper_destroy(struct sprite *s);

int handler_cat_helper_hit(struct sprite *s);
int handler_ladder_helper_hit(struct sprite *s);
int handler_ghost_helper_hit(struct sprite *s);

int handler_helper_last_level_hit(struct sprite *s);
int handler_cat_helper_last_level_hit(struct sprite *s);
int handler_ladder_helper_last_level_hit(struct sprite *s);
int handler_ghost_helper_last_level_hit(struct sprite *s);

// the platform
int handler_platform_init(struct sprite *s);
int handler_platform_frame(struct sprite *s);
int handler_platform_hit(struct sprite *s);

// spring
int handler_spring_init(struct sprite *s);
int handler_spring_frame(struct sprite *s);
int handler_spring_hit(struct sprite *s);
int handler_spring_destroy(struct sprite *s);

// skeleton
int handler_skeleton_init(struct sprite *s);
int handler_skeleton_frame(struct sprite *s);

#endif // HANDLERS_H
