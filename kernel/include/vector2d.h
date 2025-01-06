#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "math.h"

typedef struct {
  float x;
  float y;
} Vector2D;

Vector2D vector_add(Vector2D a, Vector2D b);
Vector2D vector_sub(Vector2D a, Vector2D b);
Vector2D vector_scale(Vector2D v, float scalar);
float vector_length(Vector2D v);
Vector2D vector_normalize(Vector2D v);

#endif
