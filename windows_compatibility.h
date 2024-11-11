#ifndef WINDOWS_COMPATIBILITY_H
#define WINDOWS_COMPATIBILITY_H

#ifdef _WIN32

#include <stdlib.h>

// windows does not seem to have an implementation for this function, so we
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

// windows does not seem to have an implementation for this function, so we
// add Free BSD's Open Source implementation here. This implementation, along
// with its licensing information, can be found in the Free BSD code-base here:
// https://github.com/freebsd/freebsd-src/blob/main/sys/libkern/strlcpy.c
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *__restrict dst, const char *__restrict src, size_t siz);

#endif // _WIN32
#endif // WINDOWS_COMPATIBILITY_H
