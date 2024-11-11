#ifndef LINUX_COMPATIBILITY_H
#define LINUX_COMPATIBILITY_H

#ifdef __linux__

#include <stdlib.h>

#ifndef SDL_size_mul_overflow
// currently, apt has an old version of SDL so we need to define the following
// ourselves, since it was introduced in SDL 2.24.0.
int SDL_size_mul_overflow(size_t a, size_t b, size_t *ret);

#endif // SDL_size_mul_overflow

// linux does not seem to have an implementation for this function, so we
// add Free BSD's Open Source implementation here. This implementation, along
// with its licensing information, can be found in the Free BSD code-base here:
// https://github.com/lattera/freebsd/blob/401a161083850a9a4ce916f37520c084cff1543b/contrib/mdocml/compat_strlcat.c

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t strlcat(char *dst, const char *src, size_t siz);

#endif // __linux__

#endif // LINUX_COMPATIBILITY_H
