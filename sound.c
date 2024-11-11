#include "sound.h"

#include "safe.h"
#include "state.h"
#include <stdbool.h>

// Only play sounds if this option is set to true
bool OPTIONS_SOUND_ENABLED = true;

// load_sounds loads all sounds into memory
static int load_sounds(void);

// free_sounds frees all sounds from memory
static void free_sounds(void);

int sound_create(void) {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    LOG_ERROR("%s", SDL_GetError());
    return -1;
  }

  if (Mix_Init(MIX_INIT_OGG) == 0) {
    goto mix_error;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, CHANNEL_COUNT, 2048) < 0) {
    goto mix_error;
  }

  LOG_INFO_VERBOSE("created sound state");

  if (load_sounds() != 0) {
    sound_destroy();
    return -1;
  }

  return 0;

mix_error:
  LOG_ERROR("%s", Mix_GetError());
  return -1;
}

void sound_destroy(void) {
  free_sounds();
  Mix_Quit();
  LOG_INFO_VERBOSE("destroyed sound state");
}

int sound_play(const enum sound_id id, const enum sound_channel c) {
  assert_not_null(1, g_sounds[id]);

  if (!OPTIONS_SOUND_ENABLED) {
    return 0;
  }

  if (Mix_PlayChannel(c, g_sounds[id], 0) < 0) {
    LOG_ERROR("%s", Mix_GetError());
    return -1;
  }

  return 0;
}

// --- load sounds ---

// structure to hold sound information
struct _sound_info {
  enum sound_id id;
  const char *filename;
};

static const struct _sound_info _sound_files[] = {
    {SOUND_COIN, "share/sounds/retro_coin_01.ogg"},
    {SOUND_EXTRA_LIFE, "share/sounds/sfx_sounds_powerup8.ogg"},
    {SOUND_KILL, "share/sounds/retro_die_01.ogg"},
    {SOUND_HIT, "share/sounds/sfx_damage_hit1.ogg"},
    {SOUND_MESSAGE, "share/sounds/sfx_sounds_button12.ogg"},
    {SOUND_DOOR, "share/sounds/sfx_movement_dooropen1.ogg"},
    {SOUND_JUMP, "share/sounds/sfx_movement_jump8.ogg"},
    {SOUND_SHORT_JUMP, "share/sounds/sfx_movement_jump8.ogg"},
    {SOUND_SPRING, "share/sounds/sfx_movement_jump13.ogg"},
    {SOUND_MENU_MOVE, "share/sounds/sfx_menu_move1.ogg"},
    {SOUND_MENU_SELECT, "share/sounds/sfx_menu_select2.ogg"}};

static bool load_sound(const struct _sound_info *info) {
  g_sounds[info->id] = Mix_LoadWAV(info->filename);
  if (g_sounds[info->id] == NULL) {
    LOG_ERROR("failed to load sound %s: %s", info->filename, Mix_GetError());
    return false;
  }
  return true;
}

static int load_sounds(void) {
  register size_t i;
  for (i = 0; i < sizeof(_sound_files) / sizeof(_sound_files[0]); i++) {
    if (!load_sound(&_sound_files[i])) {
      return -1;
    }
  }

  LOG_INFO_VERBOSE("all sounds loaded");
  return 0;
}

static void free_sounds(void) {
  register size_t i;

  for (i = 0; i < SOUND_COUNT; i++) {
    if (g_sounds[i] == NULL) {
      continue;
    }

    Mix_FreeChunk(g_sounds[i]);
    g_sounds[i] = NULL;
  }

  LOG_INFO_VERBOSE("all sounds freed");
}
