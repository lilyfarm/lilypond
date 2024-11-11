#include "state.h"

#include "scene.h"
#include "sound.h"
#include "sprite_type.h"

struct prog g_prog;
struct game g_game;

struct sprite_type *g_sprite_types[SPRITE_TYPE_COUNT] = {NULL};
struct scene *g_scenes[SCENE_COUNT] = {NULL};
Mix_Chunk *g_sounds[SOUND_COUNT] = {NULL};
