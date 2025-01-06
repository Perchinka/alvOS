#include "../include/asteroids.h"
#include "../include/keyboard.h"
#include "../include/screen.h"

struct GameState state;

void update_controls() {
  const bool control_states[NUM_CONTROLS] = {
      KEYBOARD_CHAR_STATE('a'), KEYBOARD_CHAR_STATE('w'),
      KEYBOARD_CHAR_STATE('d'), KEYBOARD_CHAR_STATE(' ')};

  for (size_t i = 0; i < NUM_CONTROLS; i++) {
    struct Control *c = &state.controls.raw[i];
    c->last = c->down;
    c->down = control_states[i];
    c->pressed = !c->last && c->down;

    if (c->pressed) {
      c->pressed_frames = state.frames;
    }
  }

  state.controls.rotate_left = state.controls.raw[0];
  state.controls.thrust = state.controls.raw[1];
  state.controls.rotate_right = state.controls.raw[2];
  state.controls.fire = state.controls.raw[3];
}

// Wrapping function
static void wrap_position(Vector2D *pos) {
  if (pos->x < 0)
    pos->x += SCREEN_WIDTH;
  if (pos->x >= SCREEN_WIDTH)
    pos->x -= SCREEN_WIDTH;
  if (pos->y < 0)
    pos->y += SCREEN_HEIGHT;
  if (pos->y >= SCREEN_HEIGHT)
    pos->y -= SCREEN_HEIGHT;
}

void player_update(GameObject *self, float dt) {
  // Add control-based movement here
  wrap_position(&self->position);
}

void asteroid_update(GameObject *self, float dt) {
  self->position.x += self->velocity.x * dt;
  self->position.y += self->velocity.y * dt;
  wrap_position(&self->position);
}

void bullet_update(GameObject *self, float dt) {
  self->position.x += self->velocity.x * dt;
  self->position.y += self->velocity.y * dt;
  if (self->position.x < 0 || self->position.x >= SCREEN_WIDTH ||
      self->position.y < 0 || self->position.y >= SCREEN_HEIGHT) {
    self->active = false;
  }
}

// Render stubs
void player_render(GameObject *self, void *renderer) {}
void asteroid_render(GameObject *self, void *renderer) {}
void bullet_render(GameObject *self, void *renderer) {}

void asteroids() {
  GameEngine engine;
  engine_init(&engine);
  GameObject player = {.position = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                       .velocity = {0, 0},
                       .rotation = 0,
                       .radius = 10,
                       .update = player_update,
                       .render = player_render,
                       .active = true};
  engine_add_object(&engine, player);
}
