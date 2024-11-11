#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
#include "sprite_data.h"
#include "sprite_type.h"

// sprite contains all the information relevant to model as well as render a
// sprite instance in the game.
struct sprite {
  // We use a pointer for type here because multiple sprites can be of the same
  // type, so no need to waste memory creating a duplicate type for each of
  // those sprites. As an example, let's see we have a sprite_type of TREE and
  // we have three trees in a level. Then we will have three `sprite`s and 1
  // `sprite_type`. Each of the three sprites will store the same sprite_type
  // pointer.
  struct sprite_type *type;
  struct animation animation;

  // TODO: figure out if these can be simplified to integers
  double x;  // x position
  double y;  // y position
  double vx; // x velocity
  double vy; // y velocity

  bool removed; // if true, the sprite is no longer part of the game
  // data contains data specific to particular types of sprites we would need
  // for the game logic.
  union sprite_data data;
};

// sprite_animation_set_frame sets animations of type ANIMATION_FRAME
void sprite_animation_set_frame(struct sprite *s, const int start,
                                const int end, const int fps);

// sprite_animation_set_frame_vertical sets animation of type
// ANIMATION_FRAME_VERTICAL
void sprite_animation_set_frame_vertical(struct sprite *s, const int start,
                                         const int end, const int fps);

// sprite_animation_set_flip sets animations of type ANIMATION_FLIP
void sprite_animation_set_flip(struct sprite *s, const int frame,
                               const int fps);

// sprite_init takes in a pointer to a sprite along with a
// sprite_id and sets up the default values for all its fields. Returns 0 on
// success, -1 on failure.
int sprite_init(struct sprite *s, const enum sprite_id id);

#endif // SPRITE_H
