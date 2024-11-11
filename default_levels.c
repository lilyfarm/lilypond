#include "default_levels.h"

#include "sprite_type.h"

// this file contains the level information for each default level in the
// game as well as the token entries for the custom level

const char *LEVELS[LEVEL_COUNT] = {
    "levels/small.level",  "levels/wall.level",     "levels/ground.level",
    "levels/ghost.level",  "levels/platform.level", "levels/bat.level",
    "levels/spring.level", "levels/puzzle.level",   "levels/upwards.level",
    "levels/last.level"};

// token handling for each level
enum {
  LEVEL_0_COUNT = 13,
  LEVEL_1_COUNT = 11,
  LEVEL_2_COUNT = 14,
  LEVEL_3_COUNT = 17,
  LEVEL_4_COUNT = 14,
  LEVEL_5_COUNT = 15,
  LEVEL_6_COUNT = 21,
  LEVEL_7_COUNT = CUSTOM_LEVEL_TOKEN_COUNT,
  LEVEL_8_COUNT = CUSTOM_LEVEL_TOKEN_COUNT,
  LEVEL_9_COUNT = 28,
};

const struct token_entry CUSTOM_LEVEL_TOKENS[CUSTOM_LEVEL_TOKEN_COUNT] = {
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'S', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'X', SPRITE_SPRING, token_active_sprite},
    {'k', SPRITE_SKELETON, token_active_sprite},
    {'^', SPRITE_RUST_WALL_TOP, token_passive_sprite},
    {'#', SPRITE_RUST_WALL, token_passive_sprite},
    {'b', SPRITE_BAT, token_active_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_0_TOKENS[LEVEL_0_COUNT] = {
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'h', SPRITE_CAT_HELPER, token_active_sprite},
    {'H', SPRITE_HELPER, token_active_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_1_TOKENS[LEVEL_1_COUNT] = {
    {'H', SPRITE_LADDER_HELPER, token_active_sprite},
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_2_TOKENS[LEVEL_2_COUNT] = {
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_3_TOKENS[LEVEL_3_COUNT] = {
    {'a', SPRITE_LEFT_ARROW, token_passive_sprite},
    {'H', SPRITE_GHOST_HELPER, token_active_sprite},
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'=', SPRITE_RED_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_RED_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_4_TOKENS[LEVEL_4_COUNT] = {
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'=', SPRITE_RUST_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_RUST_WALL, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'S', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_5_TOKENS[LEVEL_5_COUNT] = {
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'=', SPRITE_CAVE_TOP, token_passive_sprite},
    {'*', SPRITE_CAVE, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'b', SPRITE_BAT, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'S', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_6_TOKENS[LEVEL_6_COUNT] = {
    {'X', SPRITE_SPRING, token_active_sprite},
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'h', SPRITE_CAT_HELPER, token_active_sprite},
    {'H', SPRITE_HELPER, token_active_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SKELETON, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'S', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {'b', SPRITE_BAT, token_active_sprite},
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {' ', SPRITE_NONE, NULL}};

const struct token_entry LEVEL_9_TOKENS[LEVEL_9_COUNT] = {
    {'~', SPRITE_WATER_TOP, token_active_sprite},
    {'`', SPRITE_WATER, token_passive_sprite},
    {'L', SPRITE_LADDER, token_passive_sprite},
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'=', SPRITE_WALL_TOP, token_passive_sprite},
    {'*', SPRITE_WALL, token_passive_sprite},
    {'-', SPRITE_GROUND_TOP, token_passive_sprite},
    {'g', SPRITE_GRASS, token_passive_sprite},
    {'_', SPRITE_GROUND, token_passive_sprite},
    {'s', SPRITE_SPIDER, token_active_sprite},
    {'O', SPRITE_COIN, token_active_sprite},
    {'D', SPRITE_DOOR, token_passive_sprite},
    {'P', SPRITE_PLAYER, token_player},
    {'S', SPRITE_SPRINTING_SPIDER, token_active_sprite},
    {'G', SPRITE_GHOST, token_active_sprite},
    {'E', SPRITE_EXTRA_LIFE, token_active_sprite},
    {'p', SPRITE_PLATFORM, token_active_sprite},
    {'X', SPRITE_SPRING, token_active_sprite},
    {'k', SPRITE_SKELETON, token_active_sprite},
    {'^', SPRITE_RUST_WALL_TOP, token_passive_sprite},
    {'#', SPRITE_RUST_WALL, token_passive_sprite},
    {'b', SPRITE_BAT, token_active_sprite},
    {'H', SPRITE_HELPER_LAST_LEVEL, token_active_sprite},
    {'h', SPRITE_LADDER_HELPER_LAST_LEVEL, token_active_sprite},
    {'c', SPRITE_CAT_HELPER_LAST_LEVEL, token_active_sprite},
    {'m', SPRITE_GHOST_HELPER_LAST_LEVEL, token_active_sprite},
    {' ', SPRITE_NONE, NULL}};

#define LEVEL_7_TOKENS CUSTOM_LEVEL_TOKENS
#define LEVEL_8_TOKENS CUSTOM_LEVEL_TOKENS

const struct token_entry *LEVEL_TOKENS[LEVEL_COUNT] = {
    LEVEL_0_TOKENS, LEVEL_1_TOKENS, LEVEL_2_TOKENS, LEVEL_3_TOKENS,
    LEVEL_4_TOKENS, LEVEL_5_TOKENS, LEVEL_6_TOKENS, LEVEL_7_TOKENS,
    LEVEL_8_TOKENS, LEVEL_9_TOKENS};

// the number of token entries in the token entry array of each level
const size_t LEVEL_TOKENS_COUNT[LEVEL_COUNT] = {
    LEVEL_0_COUNT, LEVEL_1_COUNT, LEVEL_2_COUNT, LEVEL_3_COUNT, LEVEL_4_COUNT,
    LEVEL_5_COUNT, LEVEL_6_COUNT, LEVEL_7_COUNT, LEVEL_8_COUNT, LEVEL_9_COUNT};
