#ifndef ARRAY_H
#define ARRAY_H

#include "base.h"
#include "sprite.h"

#include <stdbool.h>

#define ARRAY_INIT_CAPACITY 32

// array is a dynamically allocated, automatically resized array for sprites
struct array {
  // The array internally
  struct sprite **a;
  // The length of the array
  size_t l;
  // The capacity of the array
  size_t c;
  // If this is true, variables are freed when array_clean is called.
  // By default, this is set to true. However, if the internal array a contains
  // variables which are pointers to memory allocated on the stack, then freeing
  // them will cause undefined behavior, so this should be set to false.
  bool free_on_clean;
  // If this is true, the destroy handler for a removed sprite is called when
  // array_clean is called. By default, this is set to true.
  bool destroy_on_clean;
};

// array_new creates a new sprite array with capacity ARRAY_INIT_CAPACITY
struct array *array_new(void);

// array_free frees the memory taken up by a sprite array and sets it to NULL
void array_free(struct array **pa);

// array_clean removes and frees all sprites with sprite->remove set to true
void array_clean(struct array *a);

// array_append appends a sprite to the array.
// Returns 0 on success and -1 on failure.
int array_append(struct array *a, struct sprite *s);

// array_sort sorts the array by the depth of each sprite_id (stored in
// sprite.type->id). sprite_id is an enum defined in descending order of depth.
// Take a look at enum sprite_id in base.h for more information on this.
void array_sort(struct array *a);

#endif // ARRAY_H
