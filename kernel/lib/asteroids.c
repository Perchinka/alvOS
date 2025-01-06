#include "../include/asteroids.h"
#include "../include/screen.h"
#include "../include/timer.h"

void map_controls(ControlMap *map, InputState *input) {
  map->rotate_left = &input->keys['a'];
  map->rotate_right = &input->keys['d'];
  map->thrust = &input->keys['w'];
  map->shoot = &input->keys[' '];
} // Not sure if it is the best way to do it, but it provides some scaleability

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

static int clip_line(float *x0, float *y0, float *x1, float *y1) {
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

static void draw_ship(u16 x, u16 y, float rotation, u8 color) {
  Vector2D vertices[] = {
      {10, 0},  // Tip of the triangle
      {-5, -5}, // Left base
      {-5, 5},  // Right base
  };

  float cos_r = cos(rotation);
  float sin_r = sin(rotation);

  // Transform vertices to screen coordinates
  for (int i = 0; i < 3; i++) {
    float tx = vertices[i].x * cos_r - vertices[i].y * sin_r;
    float ty = vertices[i].x * sin_r + vertices[i].y * cos_r;

    vertices[i].x = tx + x;
    vertices[i].y = ty + y;
  }

  // Draw lines connecting the vertices with clipping
  for (int i = 0; i < 3; i++) {
    float x0 = vertices[i].x;
    float y0 = vertices[i].y;
    float x1 = vertices[(i + 1) % 3].x;
    float y1 = vertices[(i + 1) % 3].y;

    // Draw if at least part of the line is visible
    if (clip_line(&x0, &y0, &x1, &y1)) {
      screen_draw_line(color, x0, y0, x1, y1);
    }
  }
}

void render_player(GameObject *player) {
  draw_ship(player->position.x, player->position.y, player->rotation, 255);
}

void update_player(GameObject *player, ControlMap *controls, float dt) {
  Vector2D direction = {cos(player->rotation), sin(player->rotation)};

  if (controls->rotate_left->is_down) {
    player->rotation -= SHIP_TURN_SPEED * dt;
  }
  if (controls->rotate_right->is_down) {
    player->rotation += SHIP_TURN_SPEED * dt;
  }

  if (player->rotation < 0) {
    player->rotation += 2.0f * PI;
  }
  if (player->rotation >= 2.0f * PI) {
    player->rotation -= 2.0f * PI;
  }

  if (controls->thrust->is_down) {
    Vector2D thrust = {cos(player->rotation), sin(player->rotation)};
    thrust = vector_normalize(thrust);
    thrust = vector_scale(thrust, SHIP_ACCELERATION * dt);
    player->velocity = vector_add(player->velocity, thrust);
  }

  // Apply friction to reduce velocity over time
  player->velocity = vector_scale(player->velocity, DAMPNING_FACTOR);

  player->position =
      vector_add(player->position, vector_scale(player->velocity, dt));
  wrap_position(&player->position, SCREEN_WIDTH, SCREEN_HEIGHT, player->radius);
}

void init_player(GameObject *player) {
  player->position = (Vector2D){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  player->velocity = (Vector2D){0, 0};
  player->rotation = 0.0f; // Measured in radians
  player->radius = 10;     // Collision radius in pixels
  player->is_active = true;

  player->update = NULL;
  player->render = render_player;
}

#define FPS 60

void game_loop() {
  GameState state;
  init_game_state(&state);

  // Initialize player
  GameObject *player = &state.objects[state.object_count++];
  init_player(player);

  // Map controls for the player
  ControlMap controls;
  map_controls(&controls, &state.input);

  u32 last_frame = 0;

  while (true) {
    const u32 now = (u32)timer_get();

    if ((now - last_frame) > (TIMER_TPS / FPS)) {
      last_frame = now;
      screen_clear(0);
      update_input(&state.input);
      update_player(player, &controls, 1.0f / FPS);
      render_game_state(&state);

      // Swap buffers or refresh the screen
    }
  }
}
