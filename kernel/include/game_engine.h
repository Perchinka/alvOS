#ifndef ENGINE_H
#define ENGINE_H

#include "vector2d.h"

#define MAX_GAME_OBJECTS 128
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

typedef struct GameObject {
  Vector2D position;
  Vector2D velocity;
  float rotation; // In radians
  float radius;   // Collision radius
  void (*update)(struct GameObject *self, float dt);
  void (*render)(struct GameObject *self, void *renderer);
  bool active; // If false, object is ignored
} GameObject;

typedef struct Control {
  bool down;
  bool last;
  bool pressed;
  size_t pressed_frames;
} Control;

typedef struct {
  Control left;
  Control right;
  Control thrust;
  Control shoot;
} InputControls;

typedef struct {
  GameObject objects[MAX_GAME_OBJECTS];
  size_t object_count;
  InputControls controls;
  size_t frames;
} GameEngine;

void engine_init(GameEngine *engine);
void engine_update(GameEngine *engine, float dt);
void engine_render(GameEngine *engine);
void engine_add_object(GameEngine *engine, GameObject object);

#endif
