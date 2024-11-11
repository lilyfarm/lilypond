#ifndef TEST_H
#define TEST_H

#include "safe.h"   // most likely needed for tests
#include <assert.h> // most likely needed for tests
#include <stdio.h>

// utilities for testing

// unix green
#define CLR_GRN "\x1B[32m"
// unix yellow
#define CLR_YEL "\x1B[33m"
// reset colors
#define CLR_RST "\x1B[0m"

// RUN_TEST prints the test that is being run, runs the test, and then prints
// "success" if the test did not fail an assertion. We assume all tests to be
// of the form:
// void test(void)
#define RUN_TEST(f)                                                            \
  do {                                                                         \
    setbuf(stdout, NULL);                                                      \
    printf("running test %s%s%s \n", CLR_YEL, #f, CLR_RST);                    \
    f();                                                                       \
    printf("%ssuccess%s.\n", CLR_GRN, CLR_RST);                                \
  } while (0)

#endif // TEST_H
