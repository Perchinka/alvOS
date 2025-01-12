#include "../include/game_engine.h"
#include "../include/screen.h"

// ----- GameState logic -----
void init_game_state(GameState *state) {
  state->object_count = 0;
  state->lives = 3;
  state->score = 0;
  state->object_count = 0;
  memset(state->objects, 0, sizeof(state->objects));
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

// ----- Animations -----
static Animation animations[MAX_ANIMATIONS];

void init_animations() {
  for (int i = 0; i < MAX_ANIMATIONS; i++) {
    animations[i].is_active = false;
  }
}

void run_animation(float duration,
                   void (*draw_callback)(float progress, void *context),
                   void *context) {
  for (int i = 0; i < MAX_ANIMATIONS; i++) {
    if (!animations[i].is_active) {
      animations[i].is_active = true;
      animations[i].duration = duration;
      animations[i].elapsed_time = 0;
      animations[i].context = context;
      animations[i].draw_callback = draw_callback;
      return;
    }
  }
}

void update_animations(float dt) {
  for (int i = 0; i < MAX_ANIMATIONS; i++) {
    if (animations[i].is_active) {
      animations[i].elapsed_time += dt;
      if (animations[i].elapsed_time >= animations[i].duration) {
        animations[i].is_active = false; // Mark animation as complete
      } else if (animations[i].draw_callback) {
        float progress = animations[i].elapsed_time / animations[i].duration;
        animations[i].draw_callback(progress, animations[i].context);
      }
    }
  }
}

// ----- Input -----
void update_input(InputState *input) {
  for (int i = 0; i < 256; i++) {
    Control *control = &input->keys[i];
    control->is_hold = control->is_down;
    control->is_down = KEYBOARD_CHAR(i);
    control->is_pressed = control->is_down && !control->is_hold;

    if (control->is_pressed) {
      control->pressed_frames = 0;
    } else if (control->is_down) {
      control->pressed_frames++;
    }
  }
}

// ---- Util functions ----

bool ellipse_circle_collision(Vector2D ellipse_center, float a, float b,
                              float rotation, Vector2D circle_center, float r) {
  float cos_r = cos(rotation);
  float sin_r = sin(rotation);

  float dx = circle_center.x - ellipse_center.x;
  float dy = circle_center.y - ellipse_center.y;

  float local_x = dx * cos_r + dy * sin_r;
  float local_y = -dx * sin_r + dy * cos_r;

  local_x /= a;
  local_y /= b;

  float distance_squared = local_x * local_x + local_y * local_y;
  float radius_scaled = r / ((a + b) / 2);

  return distance_squared <= (1 + radius_scaled) * (1 + radius_scaled);
}

bool check_collision(GameObject *a, GameObject *b) {
  float a_axis = a->size * 0.6f;
  float b_axis = a->size * 0.45f;
  return ellipse_circle_collision(a->position, a_axis, b_axis, a->rotation,
                                  b->position, b->size);
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

// ----- Wraping -----
// Liang-Barsky algorithm
#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8

static int compute_outcode(float x, float y) {
  int code = 0;
  if (x < 0)
    code |= LEFT;
  else if (x >= SCREEN_WIDTH)
    code |= RIGHT;
  if (y < 0)
    code |= BOTTOM;
  else if (y >= SCREEN_HEIGHT)
    code |= TOP;
  return code;
}

static int update_param(float p, float q, float *t0, float *t1) {
  if (p == 0.0f) {
    return q >= 0.0f; // Line is parallel and either inside or outside
  }
  float r = q / p;
  if (p < 0.0f) {
    if (r > *t1)
      return 0; // Line is outside
    if (r > *t0)
      *t0 = r;
  } else {
    if (r < *t0)
      return 0; // Line is outside
    if (r < *t1)
      *t1 = r;
  }
  return 1;
}

int clip_line(float *x0, float *y0, float *x1, float *y1) {
  float t0 = 0.0f, t1 = 1.0f; // Parametric range for visible segment
  float dx = *x1 - *x0, dy = *y1 - *y0;

  if (!update_param(-dx, *x0, &t0, &t1))
    return 0; // Left
  if (!update_param(dx, SCREEN_WIDTH - 1 - *x0, &t0, &t1))
    return 0; // Right
  if (!update_param(-dy, *y0, &t0, &t1))
    return 0; // Bottom
  if (!update_param(dy, SCREEN_HEIGHT - 1 - *y0, &t0, &t1))
    return 0; // Top

  if (t1 < 1.0f) {
    *x1 = *x0 + t1 * dx;
    *y1 = *y0 + t1 * dy;
  }
  if (t0 > 0.0f) {
    *x0 = *x0 + t0 * dx;
    *y0 = *y0 + t0 * dy;
  }

  return 1; // Line is visible
}

// Used for explosion animation
bool clip_circle(float x, float y, float radius) {
  return x + radius >= 0 && x - radius < SCREEN_WIDTH && y + radius >= 0 &&
         y - radius < SCREEN_HEIGHT;
}
