#include "array.h"

#include "test.h"

static void test_array_allocation(void) {
  struct array *a = array_new();
  assert(a != NULL);
  assert(a->l == 0);

  array_free(&a);
  assert(a == NULL);
}

static void test_array_append(void) {
  struct array *a = array_new();
  assert(a != NULL);

  a->free_on_clean = false;
  a->destroy_on_clean = false;

  struct sprite s;
  struct sprite s2;

  array_append(a, &s);

  assert(a->l == 1);
  assert(a->a[0] == &s);
  assert(a->a[0] != &s2);

  array_append(a, &s2);

  assert(a->l == 2);
  assert(a->a[0] == &s);
  assert(a->a[1] == &s2);

  register size_t i;
  for (i = 0; i < ARRAY_INIT_CAPACITY; i++) {
    array_append(a, &s);
  }

  assert(a->l == ARRAY_INIT_CAPACITY + 2);
  assert(a->c > ARRAY_INIT_CAPACITY + 2);

  array_free(&a);
}

static void test_array_clean(void) {
  struct array *a = array_new();
  assert(a != NULL);

  // Since the sprites we will be using are not dynamically allocated.
  a->free_on_clean = false;
  a->destroy_on_clean = false;

  struct sprite s1, s2, s3;

  s1.removed = false;
  s2.removed = true;
  s3.removed = false;

  array_append(a, &s1);
  array_append(a, &s2);
  array_append(a, &s3);

  array_clean(a);

  assert(a->l == 2);
  assert(a->a[0] == &s1);
  assert(a->a[1] == &s3);

  s1.removed = true;
  s3.removed = true;

  array_clean(a);
  assert(a->l == 0);

  array_free(&a);
}

static void test_array_sort(void) {
  // we trust qsort here. Just checking if our comparison function does what we
  // think it does.
  struct array *a = array_new();
  assert(a != NULL);

  a->free_on_clean = false;
  a->destroy_on_clean = false;

  struct sprite s1, s2;
  struct sprite_type t1, t2;

  s1.type = &t1;
  s2.type = &t2;

  t1.id = SPRITE_TEST_LO_DEPTH;
  t2.id = SPRITE_TEST_HI_DEPTH;

  array_append(a, &s1);
  array_append(a, &s2);

  assert(a->a[0]->type->id == SPRITE_TEST_LO_DEPTH);
  assert(a->a[1]->type->id == SPRITE_TEST_HI_DEPTH);

  array_sort(a);

  assert(a->a[0]->type->id == SPRITE_TEST_HI_DEPTH);
  assert(a->a[1]->type->id == SPRITE_TEST_LO_DEPTH);

  array_free(&a);
}

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  RUN_TEST(test_array_allocation);
  RUN_TEST(test_array_append);
  RUN_TEST(test_array_clean);
  RUN_TEST(test_array_sort);

  return EXIT_SUCCESS;
}
