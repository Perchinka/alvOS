#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../include/keyboard.h"
#include "vector2d.h"

#define MAX_GAME_OBJECTS 128
#define MAX_VERTICES 16

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

typedef struct Control {
  bool is_down;          // Is the key currently pressed?
  bool is_hold;          // Was the key pressed in the previous frame?
  bool is_pressed;       // Was the key just pressed this frame?
  size_t pressed_frames; // Number of frames the key has been held
} Control;

typedef struct InputState {
  Control keys[256]; // One control for each possible key
} InputState;

typedef struct GameObject {
  Vector2D position;
  Vector2D velocity;
  float rotation; // TODO Update later to be a Transform struct instead of 3
                  // independant fields
  float rotation_speed;
  float size; // Used for collisions everything is circle for now
  bool is_active;

  Vector2D vertices[MAX_VERTICES]; // Kind of sprite, also it can be used for
                                   // collisions detection
  int vertex_count;

  void (*update)(struct GameObject *self, float dt);
  void (*render)(struct GameObject *self);
} GameObject;

typedef struct GameState {
  GameObject objects[MAX_GAME_OBJECTS];
  size_t object_count;
  InputState input;
} GameState;

void init_game_state(GameState *state);
void update_game_state(GameState *state, float dt);
void render_game_state(GameState *state);

void update_input(InputState *input);

bool check_collision(GameObject *a, GameObject *b);
void wrap_position(Vector2D *position, int screen_width, int screen_height,
                   int additional_size);
int clip_line(float *x0, float *y0, float *x1, float *y1);

#endif
