#ifndef SAFE_H
#define SAFE_H

#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>

#define SAFE_VERBOSE_INFO

#define SAFE_UNUSED(x) (void)(x)

// this file contains utility functions relevant to writing safe, bug-free, and
// debuggable code

// size_t_to_int safely converts size_t to int, and asserts that no information
// is lost.
int size_t_to_int(const size_t value);

// assert_not_null takes in a variable list of pointers and asserts that each of
// them are not NULL. This is a nice utility function we will be using
// throughout the code base
void _assert_not_null(int num_args, ...);

#ifdef SAFE_VERBOSE_ASSERT
#define assert_not_null(...)                                                   \
  do {                                                                         \
    LOG_ERROR("checking assertion");                                           \
    _assert_not_null(__VA_ARGS__);                                             \
    LOG_ERROR("passed");                                                       \
  } while (0)
#else
#define assert_not_null(...) _assert_not_null(__VA_ARGS__)
#endif

// LOG_ERROR is a utility function to log errors
#define LOG_ERROR(...)                                                         \
  do {                                                                         \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,                                 \
                 "error metadata: function [%s], line [%d], file [%s]",        \
                 __func__, __LINE__, __FILE__);                                \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);                   \
  } while (0)

// LOG_DEBUG is a utility function to log debug information
#ifdef SAFE_DEBUG
#define LOG_DEBUG(...)                                                         \
  do {                                                                         \
    printf("[DEBUG]");                                                         \
    LOG_ERROR(__VA_ARGS__);                                                    \
  } while (0)
#else
#define LOG_DEBUG(...)
#endif

// LOG_INFO does logging at an information level. We try to avoid any
// non-user-friendly information here, keeping detailed SDL logging for
// LOG_ERROR and LOG_DEBUG. This is information targeted at the *user* to see if
// the game loaded correctly or find ways to unblock themselves. This is not
// targeted at us, the programmers of the game.
#define LOG_INFO(...)                                                          \
  do {                                                                         \
    printf("[LOG] ");                                                          \
    printf(__VA_ARGS__);                                                       \
    printf("\n");                                                              \
    fflush(stdout);                                                            \
  } while (0)
#endif // SAFE_H
// LOG_INFO_VERBOSE is logging at an information level that is user facing (just
// like LOG_INFO but more verbose than is needed for most uses of the game. This
// is intended for expert users trying to, for example, mod the game by adding
// their own levels etc.
#ifdef SAFE_VERBOSE_INFO
#define LOG_INFO_VERBOSE(...)                                                  \
  do {                                                                         \
    printf("[VERBOSE]");                                                       \
    LOG_INFO(__VA_ARGS__);                                                     \
  } while (0)
#else
#define LOG_INFO_VERBOSE(...)
#endif
