#include <string.h> // Ensure there is at least one declaration

#if defined(__EMSCRIPTEN__) || defined(__linux__)

#include "emscripten_compatibility.h"

size_t strnlen(const char *s, size_t maxlen) {
  size_t len;

  for (len = 0; len < maxlen; len++, s++) {
    if (!*s)
      break;
  }
  return (len);
}

size_t strlcpy(char *__restrict dst, const char *__restrict src, size_t siz) {
  char *d = dst;
  const char *s = src;
  size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0) {
    while (--n != 0) {
      if ((*d++ = *s++) == '\0')
        break;
    }
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (siz != 0)
      *d = '\0'; /* NUL-terminate dst */
    while (*s++)
      ;
  }

  return (s - src - 1); /* count does not include NUL */
}
#endif // __EMSCRIPTEN__ || __linux__
