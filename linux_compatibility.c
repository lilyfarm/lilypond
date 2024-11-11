#include "linux_compatibility.h"

#include <string.h> // ensure there is at least one declaration

#ifdef __linux__

#ifndef SDL_SIZE_MAX
#ifdef SIZE_MAX
#define SDL_SIZE_MAX SIZE_MAX
#else
#define SDL_SIZE_MAX ((size_t)-1)
#endif
#endif

#ifndef SDL_size_mul_overflow
int SDL_size_mul_overflow(size_t a, size_t b, size_t *ret) {
  if (a != 0 && b > SDL_SIZE_MAX / a) {
    return -1;
  }
  *ret = a * b;
  return 0;
}

size_t strlcat(char *dst, const char *src, size_t siz) {
  char *d = dst;
  const char *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != '\0')
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0)
    return (dlen + strlen(s));
  while (*s != '\0') {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return (dlen + (s - src)); /* count does not include NUL */
}

#endif // SDL_size_mul_overflow

#endif // __linux__
