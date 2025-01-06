#include "../include/math.h"

f64 fmod(f64 x, f64 y) {
  if (y == 0.0f) {
    return 0.0f;
  }

  int quotient = (int)(x / y);
  return x - quotient * y;
}

f64 sin(f64 x) {
  f64 result;
  asm("fsin" : "=t"(result) : "0"(x));
  return result;
}

f64 cos(f64 x) {
  f64 result;
  asm("fcos" : "=t"(result) : "0"(x));
  return result;
}

double sqrt(double n) {
  if (n < 0) {
    return -1; // Return -1 for invalid input (negative numbers)
  }
  if (n == 0 || n == 1) {
    return n; // Square root of 0 or 1 is the number itself
  }

  double guess = n / 2.0;    // Initial guess
  double epsilon = 0.000001; // Precision threshold

  // Newton's method
  while ((guess * guess - n) > epsilon || (n - guess * guess) > epsilon) {
    guess = (guess + n / guess) / 2.0;
  }

  return guess;
}

f64 pow(f64 base, f64 exp) {
  if (base == 0.0)
    return 0.0; // 0^x = 0 (except 0^0, handled separately)
  if (exp == 0.0)
    return 1.0; // x^0 = 1

  f64 result;
  asm("fldln2;"      // Load ln(2) onto the FPU stack
      "fld %2;"      // Push exp (y) onto the FPU stack
      "fld %1;"      // Push base (x) onto the FPU stack
      "fyl2x;"       // Compute y * log2(x), result on FPU stack
      "fld %%st(0);" // Duplicate result (logarithm part)
      "frndint;" // Round to integer for exponent part "fsub %%st(1), %%st;" //
                 // Subtract integer part to get fractional part
      "fxch;"    // Swap to process fractional part
      "f2xm1;"   // Compute 2^(fractional part) - 1
      "fld1;"    // Load 1 onto the stack
      "faddp;"   // Add 1, result is 2^(fractional part)
      "fscale;"  // Scale by 2^(integer part)
      "fstp %%st(1);"       // Remove integer part from stack
      : "=t"(result)        // Output to result
      : "m"(base), "m"(exp) // Input base and exp
  );
  return result;
}

f64 log(f64 x) {
  if (x <= 0.0) {
    return -1.0; // Logarithm undefined for non-positive values
  }

  f64 result;
  asm("fldln2;"      // Load ln(2) onto the FPU stack
      "fld %1;"      // Push x onto the FPU stack
      "fyl2x;"       // Compute y * log2(x), result = ln(x)
      : "=t"(result) // Output to result
      : "m"(x)       // Input x
  );
  return result;
}

f64 atan2(f64 y, f64 x) {
  f64 result;
  asm("fld %1;"        // Push y onto the FPU stack
      "fld %2;"        // Push x onto the FPU stack
      "fpatan;"        // Compute atan(y / x)
      : "=t"(result)   // Output to result
      : "m"(y), "m"(x) // Input y and x
  );
  return result;
}
