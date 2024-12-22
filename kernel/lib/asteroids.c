#include "../include/asteroids.h"
#include "../include/keyboard.h"
#include "../include/math.h"
#include "../include/screen.h"

Ship ship = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0, 0, 0, 0}; // Initial state

#define NUM_CONTROLS 4

struct Control {
  bool down;             // Current state
  bool last;             // Previous state
  bool pressed;          // True if pressed this frame
  size_t pressed_frames; // Frame count when pressed
};

struct GameState {
  struct {
    struct Control raw[NUM_CONTROLS]; // Array of all controls
    struct Control thrust;
    struct Control rotate_left;
    struct Control rotate_right;
    struct Control fire;
  } controls;
  size_t frames;
};

struct GameState state;

void update_controls() {
  // Define the mapping of keys to controls
  const bool control_states[NUM_CONTROLS] = {
      KEYBOARD_CHAR_STATE('a'), KEYBOARD_CHAR_STATE('w'),
      KEYBOARD_CHAR_STATE('d'), KEYBOARD_CHAR_STATE(' ')};

  // Update control states
  for (size_t i = 0; i < NUM_CONTROLS; i++) {
    struct Control *c = &state.controls.raw[i];
    c->last = c->down;
    c->down = control_states[i];
    c->pressed = !c->last && c->down;

    if (c->pressed) {
      c->pressed_frames = state.frames;
    }
  }

  // Map individual controls to specific actions
  state.controls.rotate_left = state.controls.raw[0];
  state.controls.thrust = state.controls.raw[1];
  state.controls.rotate_right = state.controls.raw[2];
  state.controls.fire = state.controls.raw[3];
}
void update_game_state() {
  update_controls();

  if (state.controls.rotate_left.down) {
    ship.angle -= SHIP_TURN_SPEED;
  }
  if (state.controls.rotate_right.down) {
    ship.angle += SHIP_TURN_SPEED;
  }

  if (state.controls.thrust.down) {
    ship.vx += cos(ship.angle) * SHIP_ACCELERATION;
    ship.vy += sin(ship.angle) * SHIP_ACCELERATION;
  } else {
    ship.vx *= 0.99999f; // Damping factor
    ship.vy *= 0.99999f;
  }

  float speed = sqrt(ship.vx * ship.vx + ship.vy * ship.vy);
  if (speed > MAX_SHIP_SPEED) {
    float scale = MAX_SHIP_SPEED / speed;
    ship.vx *= scale;
    ship.vy *= scale;
  }
  ship.x += CLAMP(ship.vx, -MAX_SHIP_SPEED, MAX_SHIP_SPEED);
  ship.y += CLAMP(ship.vy, -MAX_SHIP_SPEED, MAX_SHIP_SPEED);

  if (ship.x < -40)
    ship.x += 400;
  if (ship.x >= 400)
    ship.x -= 440;
  if (ship.y < 40)
    ship.y += 320;
  if (ship.y >= 320)
    ship.y -= 320;
  state.frames++;
}

void render_game_state() {
  screen_clear(0);

  float nose_x = ship.x + cos(ship.angle) * SHIP_SIZE;
  float nose_y = ship.y + sin(ship.angle) * SHIP_SIZE;
  float left_x = ship.x + cos(ship.angle + 2 * PI / 3) * SHIP_SIZE;
  float left_y = ship.y + sin(ship.angle + 2 * PI / 3) * SHIP_SIZE;
  float right_x = ship.x + cos(ship.angle - 2 * PI / 3) * SHIP_SIZE;
  float right_y = ship.y + sin(ship.angle - 2 * PI / 3) * SHIP_SIZE;

  screen_draw_line(255, (u16)nose_x, (u16)nose_y, (u16)left_x, (u16)left_y);
  screen_draw_line(255, (u16)left_x, (u16)left_y, (u16)right_x, (u16)right_y);
  screen_draw_line(255, (u16)right_x, (u16)right_y, (u16)nose_x, (u16)nose_y);

  float marker_x = ship.x + cos(ship.angle) * (SHIP_SIZE + 5);
  float marker_y = ship.y + sin(ship.angle) * (SHIP_SIZE + 5);

  screen_draw_line(200, (u16)ship.x, (u16)ship.y, (u16)marker_x, (u16)marker_y);
}
