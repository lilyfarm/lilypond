#ifndef CAMERA_H
#define CAMERA_H

#include "base.h"

#include "sprite.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// camera_create initializes a camera based on a sprite (typically the player's
// sprite) and a level width (see level.w).
// c: the camera
// s: the sprite
// w: the level width
// h: the level height
void camera_create(SDL_Rect *c, const struct sprite *s, const size_t w,
                   const size_t h);

// camera_update updates the dimensions of a camera based on a sprite (typically
// the player's sprite) and a level width (see level.w).
// c: the camera
// s: the sprite
// w: the level width
// h: the level height
void camera_update(SDL_Rect *c, const struct sprite *s, const size_t w,
                   const size_t h);

// camera_map maps a destination rect based on where the current camera is.
// Returns true is the rect is within the confines of the screen after being
// mapped, and false otherwise.
// c: the camera
// dst: the destination rect
bool camera_map(const SDL_Rect *c, SDL_Rect *dst);

#endif // CAMERA_H
