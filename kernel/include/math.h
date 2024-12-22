#ifndef MATH_H
#define MATH_H

#define CLAMP(_value, _min, _max)                                              \
  (((_value) < (_min)) ? (_min) : ((_value) > (_max)) ? (_max) : (_value))

#endif
