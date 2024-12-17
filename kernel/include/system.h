#ifndef SYSTEM_H
#define SYSTEM_H

#include "utils.h"

// Assertion Macros
#define _assert_0() static_assert(0, "assert() requires at least one argument")
#define _assert_1(_e)                                                          \
  do {                                                                         \
    if (!(_e))                                                                 \
      panic("Assertion failed: " #_e);                                         \
  } while (0)
#define _assert_2(_e, _m)                                                      \
  do {                                                                         \
    if (!(_e))                                                                 \
      panic(_m);                                                               \
  } while (0)

// Macro overloading
#define GET_MACRO(_1, _2, NAME, ...) NAME
#define assert(...)                                                            \
  GET_MACRO(__VA_ARGS__, _assert_2, _assert_1, _assert_0)(__VA_ARGS__)

void panic(const char *err);

u32 rand();
void seed(u32 s);

#endif
