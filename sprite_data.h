#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "fps.h"

// direction contains horizontal directions, used for util functions and sprite
// movement
enum direction { DIR_LEFT, DIR_RIGHT };

// data_item contains the data necessary to process an item
struct data_item {
  // collected is true if the item has been collected by the player
  bool collected;
};

// data_enemy contains the data necessary to process an enemy sprite
struct data_enemy {
  enum direction dir;
  bool alive;
  struct fps_timer *remove_timer;
  struct fps_timer *shoot_timer;
};

// data_helper contains the data necessary to process a helper character
struct data_helper {
  struct fps_timer *interaction_timer;
};

// sprite_data contains sprite type specific information we will need for the
// game. A union is the best data structure we could think of for this.
union sprite_data {
  struct data_item item;
  struct data_enemy enemy;
  struct data_helper helper;
};

#endif // SPRITE_DATA_H
