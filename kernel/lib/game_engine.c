#include "../include/game_engine.h"

// ----- GameState logic -----
void init_game_state(GameState *state) {
  state->object_count = 0;
  for (int i = 0; i < 256; i++) {
    state->input.keys[i] = (Control){0};
  }
}

void update_game_state(GameState *state, float dt) {
  for (size_t i = 0; i < state->object_count; i++) {
    GameObject *obj = &state->objects[i];
    if (obj->is_active && obj->update) {
      obj->update(obj, dt);
    }
  }
}

void render_game_state(GameState *state) {
  for (size_t i = 0; i < state->object_count; i++) {
    GameObject *obj = &state->objects[i];
    if (obj->is_active && obj->render) {
      obj->render(obj);
    }
  }
}

// ----- Input -----
void update_input(InputState *input) {
  for (int i = 0; i < 256; i++) {
    Control *control = &input->keys[i];
    control->is_hold = control->is_down;
    control->is_down = KEYBOARD_CHAR_STATE(i);
    control->is_pressed = control->is_down && !control->is_hold;

    if (control->is_pressed) {
      control->pressed_frames = 0;
    } else if (control->is_down) {
      control->pressed_frames++;
    }
  }
}

// ---- Util functions ----
bool check_collision(GameObject *a, GameObject *b) {
  Vector2D delta = vector_sub(a->position, b->position);
  float distance = vector_length(delta);
  return distance < (a->size + b->size);
}

void wrap_position(Vector2D *position, int screen_width, int screen_height,
                   int additional_size) {
  if (position->x < -additional_size)
    position->x = screen_width + additional_size - 1;
  if (position->x >= screen_width + additional_size)
    position->x = -additional_size + 1;
  if (position->y < -additional_size)
    position->y = screen_height + additional_size - 1;
  if (position->y >= screen_height + additional_size)
    position->y = -additional_size + 1;
}
