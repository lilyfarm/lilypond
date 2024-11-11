#include "array.h"

#include "safe.h"
#include <errno.h>

struct array *array_new(void) {
  struct array *a = (struct array *)malloc(sizeof(struct array));

  if (a == NULL) {
    // errno will automatically be set to ENOMEM
    return NULL;
  }

  a->c = ARRAY_INIT_CAPACITY;
  a->l = 0;
  a->free_on_clean = true;
  a->destroy_on_clean = true;

  // Some implementation of calloc, especially older ones such as the 4.4 BSD
  // implementation [1] do *not* check for multiplication overflow. So we do it
  // here. [1]:
  // https://minnie.tuhs.org/cgi-bin/utree.pl?file=4.4BSD/usr/src/lib/libc/stdlib/calloc.c
  size_t capacity;
  int ret = SDL_size_mul_overflow(sizeof(struct sprite *), a->c, &capacity);
  if (ret == -1) {
    errno = EOVERFLOW;
    free(a);
    return NULL;
  }

  a->a = (struct sprite **)calloc(1, capacity);
  if (a->a == NULL) {
    // errno will automatically be set to ENOMEM
    free(a);
    return NULL;
  }

  return a;
}

void array_free(struct array **pa) {
  assert_not_null(2, pa, *pa);
  struct array *a = *pa;

  // free all the sprites inside the array
  register size_t i;
  for (i = 0; i < a->l; i++) {
    struct sprite *s = a->a[i];
    s->removed = true;
  }

  array_clean(a);

  free(a->a);
  free(a);
  *pa = NULL;
}

void array_clean(struct array *a) {
  assert_not_null(1, a);

  size_t r = 0;

  register size_t i;
  for (i = 0; i < a->l; i++) {
    struct sprite *s = a->a[i];
    if (s->removed) {
      if (a->destroy_on_clean) {
        // call the destroy handler of the sprite. It is okay to make this a
        // compile time assertion as deallocation is usually a fail-safe
        // operation (e.g the free function).
        assert(s->type->destroy_handler(s) == 0);
      }

      if (a->free_on_clean) {
        free(s);
      }

      r += 1;
    } else if (r) {
      a->a[i - r] = s;
      a->a[i] = NULL;
    }
  }

  a->l -= r;
}

int array_append(struct array *a, struct sprite *s) {
  assert_not_null(2, a, s);

  // If we have reached our capacity, then double the capacity
  // and reallocate. Being careful about checking for overflow.
  if (a->l == a->c) {
    size_t new_c;
    int ret = SDL_size_mul_overflow(a->c, 2, &new_c);

    // We overflowed
    if (ret == -1) {
      errno = EOVERFLOW;
      return -1;
    }

    // If there was no overflow, let's try reallocating to the new capacity.
    size_t capacity;
    ret = SDL_size_mul_overflow(sizeof(struct sprite *), new_c, &capacity);

    // We overflowed
    if (ret == -1) {
      errno = EOVERFLOW;
      return -1;
    }

    struct sprite **new_a = (struct sprite **)realloc(a->a, capacity);

    // reallocation failed
    if (new_a == NULL) {
      // errno = ENOMEM;
      return -1;
    }

    // We are finally ready to actually make state changes
    a->a = new_a;
    a->c = new_c;
  }

  a->a[a->l] = s;
  a->l++;
  return 0;
}

// sprite_compare is used for qsort in array_sort
static int sprite_compare(const void *a, const void *b) {
  assert_not_null(2, a, b);
  const struct sprite *sa = *(const struct sprite **)a;
  const struct sprite *sb = *(const struct sprite **)b;

  return (sb->type->id - sa->type->id);
}

void array_sort(struct array *a) {
  assert_not_null(1, a);
  qsort(a->a, a->l, sizeof(struct sprite *), sprite_compare);
}
