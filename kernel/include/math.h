#ifndef MATH_H
#define MATH_H

#include "utils.h"

#define E 2.7182818284590452353602874713527
#define PI 3.14159265358979323846264338327950
#define HALF_PI (PI / 2.0)

#define CLAMP(_value, _min, _max)                                              \
  (((_value) < (_min)) ? (_min) : ((_value) > (_max)) ? (_max) : (_value))

static inline int abs(int value) { return value < 0 ? -value : value; }

f64 fmod(f64 x, f64 m);
f64 sin(f64 x);
f64 cos(f64 x);
f64 pow(f64 x, f64 y);
f64 log(f64 x);
f64 atan2(f64 y, f64 x);
double sqrt(double n);

#endif // MATH_H
