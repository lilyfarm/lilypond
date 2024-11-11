#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>

// animation_type contains the types of ways to animate all sprites in the
// game
enum animation_type {
  // ANIMATION_FRAME just animates sprites frame by frame
  ANIMATION_FRAME,
  // ANIMATION_FRAME_VERTICAL animates sprites frame by frame but the frames are
  // arranged vertically
  ANIMATION_FRAME_VERTICAL,
  // ANIMATION_FLIP horizontally flips the sprite
  ANIMATION_FLIP
};

// animation contains all the state necessary to animate a sprite
struct animation {
  enum animation_type type;
  Sint64 frame;
  Sint64 frame_start;
  Sint64 frame_end;
  Sint64 frame_delay;         // frame_delay is in milliseconds
  Sint64 frame_delay_counter; // frame_delay_counter is in milliseconds
  SDL_RendererFlip flip;
  Sint64 alpha; // The alpha channel i.e how visible a sprite is. (see
                // SDL_SetAlpha)
};

#endif // ANIMATION_H
