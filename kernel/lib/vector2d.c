#include "../include/vector2d.h"

Vector2D vector_add(Vector2D a, Vector2D b) {
  return (Vector2D){a.x + b.x, a.y + b.y};
}

Vector2D vector_sub(Vector2D a, Vector2D b) {
  return (Vector2D){a.x - b.x, a.y - b.y};
}

Vector2D vector_scale(Vector2D v, float scalar) {
  return (Vector2D){v.x * scalar, v.y * scalar};
}

float vector_length(Vector2D v) { return sqrt(v.x * v.x + v.y * v.y); }

Vector2D vector_normalize(Vector2D v) {
  float length = vector_length(v);
  return length > 0 ? vector_scale(v, 1.0f / length) : (Vector2D){0, 0};
}
