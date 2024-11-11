#ifndef BASE_H
#define BASE_H

#ifdef __EMSCRIPTEN__
#include "emscripten_compatibility.h"
#endif // __EMSCRIPTEN__

#ifdef _WIN32
#include "windows_compatibility.h"
#endif // _WIN32

#ifdef __linux__
#include "emscripten_compatibility.h"
#include "linux_compatibility.h"
#endif // __linux__

// the maximum velocity at which the player or other sprites can move
extern const double MAX_VELOCITY;

// SIZE_FACTOR is what every tile will be multiplied by prior to rendering it.
#define SIZE_FACTOR 2
// LEVEL_WIDTH is the width of the screen in terms of *logical* pixels. To
// translate this into *real* pixels, multiply by SIZE_FACTOR.
#define LEVEL_WIDTH 320
// LEVEL_HEIGHT is the height of the screen in terms of *logical* pixels. To
// translate this into *real* pixels, multiply by SIZE_FACTOR.
#define LEVEL_HEIGHT 240
// SPRITE_SIZE is the size (width and height, in pixels) of each sprite in the
// sprite sheet.
#define SPRITE_SIZE 16
// ROW_COUNT is the number of rows on the screen i.e in a level.
#define ROW_COUNT ((LEVEL_HEIGHT + SPRITE_SIZE - 1) / SPRITE_SIZE)
// COLUMN_COUNT is the number of columns on the streen i.e in a level.
#define COLUMN_COUNT ((LEVEL_WIDTH + SPRITE_SIZE - 1) / SPRITE_SIZE)
// SPRITE_COUNT is the number of sprites on the screen i.e in a level.
#define SPRITE_COUNT (ROW_COUNT * COLUMN_COUNT)
// TEXT_WIDTH is the width of the text box underneath the game. We will be using
// it to communicate to users.
#define TEXT_WIDTH LEVEL_WIDTH // same as LEVEL_WIDTH
// TEXT_HEIGHT is the height of the text box underneath the game. We will be
// using it to communicate to users.
#define TEXT_HEIGHT 60

#endif // BASE_H
