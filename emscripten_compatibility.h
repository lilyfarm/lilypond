#ifndef EMSCRIPTEN_COMPATIBILITY_H
#define EMSCRIPTEN_COMPATIBILITY_H

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#if defined(__EMSCRIPTEN__) || defined(__linux__)
#include <stdlib.h>

// emscripten does not seem to have an implementation for this function, so we
// add Free BSD's Open Source implementation here. This implementation, along
// with its licensing information, can be found in the Free BSD code-base here:
// https://github.com/freebsd/freebsd-src/blob/main/sys/libkern/strnlen.c
size_t strnlen(const char *s, size_t maxlen);

// emscripten does not seem to have an implementation for this function, so we
// add Free BSD's Open Source implementation here. This implementation, along
// with its licensing information, can be found in the Free BSD code-base here:
// https://github.com/freebsd/freebsd-src/blob/main/sys/libkern/strlcpy.c
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *__restrict dst, const char *__restrict src, size_t siz);

#endif // __EMSCRIPTEN__ || __linux__

#endif // EMSCRIPTEN_COMPATIBILITY_H
