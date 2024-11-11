#include "safe.h"

#include <assert.h>
#include <limits.h>
#include <stdarg.h>

int size_t_to_int(const size_t value) {
  assert(value <= INT_MAX);
  return (int)value;
}

void _assert_not_null(int num_args, ...) {
  va_list args;
  va_start(args, num_args);

  for (int i = 0; i < num_args; i++) {
    const void *ptr = va_arg(args, const void *);
    assert(ptr != NULL);
  }

  va_end(args);
}
