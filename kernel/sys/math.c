#include "../include/math.h"

f64 sin(f64 x) {
  f64 result;
  asm("fsin" : "=t"(result) : "0"(x));
  return result;
}

f64 cos(f64 x) { return sin(x + PI / 2.0); }

double sqrt(double n) {
  if (n < 0) {
    return -1; // Return -1 for invalid input (negative numbers)
  }
  if (n == 0 || n == 1) {
    return n; // Square root of 0 or 1 is the number itself
  }

  double guess = n / 2.0;    // Initial guess
  double epsilon = 0.000001; // Precision threshold

  while ((guess * guess - n) > epsilon || (n - guess * guess) > epsilon) {
    guess = (guess + n / guess) / 2.0;
  }

  return guess;
}
