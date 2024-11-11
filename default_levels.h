#ifndef SCENE_GAME_LEVELS_H
#define SCENE_GAME_LEVELS_H

#include "token.h"
#include <stdlib.h>

//  declarations for all variables stored in default_levels.c

enum {
  LEVEL_COUNT = 10,
  CUSTOM_LEVEL_TOKEN_COUNT = 23,
};
extern const char *LEVELS[LEVEL_COUNT];
extern const struct token_entry *LEVEL_TOKENS[LEVEL_COUNT];
extern const struct token_entry CUSTOM_LEVEL_TOKENS[CUSTOM_LEVEL_TOKEN_COUNT];
extern const size_t LEVEL_TOKENS_COUNT[LEVEL_COUNT];

#endif
