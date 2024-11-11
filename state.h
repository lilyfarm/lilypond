#ifndef STATE_H
#define STATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// contains all the global state of the game

// prog_state enumerates all the (valid) states in which the program can be
// in.
enum prog_state {
  // the game has not started yet
  PROG_NOT_STARTED,
  // the user quit the program
  PROG_EXIT,
  // the program is exiting due to an error
  PROG_ERROR,
  // the user is in the game i.e playing it
  PROG_GAME_IN,
  // the player was killed
  PROG_GAME_KILLED,
  // the user lost the game
  PROG_GAME_OVER,
  // the user completed the game
  PROG_GAME_COMPLETE,
  // the user completed a level in the game
  PROG_GAME_LEVEL_COMPLETE
};

// Some forward declarations so we can hold pointers in the game and prog
// structs
struct level;  // see level.h
struct player; // see player.h
struct scene;  // see scene.h

// prog contains program-wide state
struct prog {
  enum prog_state state; // the state of the program
  const Uint8 *keys;     // the state of the keyboard
  struct scene *scene;   // the current scene
};

struct game {
  // the time, in milliseconds, in between cleaning removed active sprites
  Sint64 clean;

  // level keeps track of the current level the player is on
  struct level *level;
  struct player *player;   // the player
  struct message *message; // the game message

  // path to a custom level, in case we want to run a user-created level
  char *custom_level_path;

  // camera helps keep track of the part of the level to render, based on the
  // position of the player. We use this to implement side-scrolling
  // functionality. See scene.c for uses of the camera for rendering.
  SDL_Rect camera;
};

// the global game state
extern struct game g_game;

// the global program state
extern struct prog g_prog;

// g_sounds is an array of Mix_Chunk* pointers to store all the sound effects
// used in the game. Used in sound.h and sound.c
extern Mix_Chunk *g_sounds[];

// Forward declaration. See sprite_type.h and sprite_type.c
struct sprite_type;

// g_sprite_types is a global static array containing all the sprite_types
// declared. Each sprite instance points to one of these sprite_types.
extern struct sprite_type *g_sprite_types[];

// g_scenes is a global static array containing all the scenes declared.
extern struct scene *g_scenes[];

#endif // STATE_H
