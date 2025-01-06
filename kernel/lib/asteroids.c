#include "../include/asteroids.h"
#include "../include/screen.h"
#include "../include/timer.h"

void map_controls(ControlMap *map, InputState *input) {
  map->rotate_left = &input->keys['a'];
  map->rotate_right = &input->keys['d'];
  map->thrust = &input->keys['w'];
  map->shoot = &input->keys[' '];
} // Not sure if it is the best way to do it, but it provides some scaleability

static void clamp_to_screen(Vector2D *vertex) {
  if (vertex->x < 0)
    vertex->x = 0;
  if (vertex->x >= SCREEN_WIDTH)
    vertex->x = SCREEN_WIDTH - 1;
  if (vertex->y < 0)
    vertex->y = 0;
  if (vertex->y >= SCREEN_HEIGHT)
    vertex->y = SCREEN_HEIGHT - 1;
}

static void draw_ship(u16 x, u16 y, float rotation, u8 color) {
  Vector2D vertices[] = {
      {10, 0},  // Tip of the triangle
      {-5, -5}, // Left base
      {-5, 5},  // Right base
  };

  float cos_r = cos(rotation);
  float sin_r = sin(rotation);

  // Transform and translate vertices
  for (int i = 0; i < 3; i++) {
    float tx = vertices[i].x * cos_r - vertices[i].y * sin_r;
    float ty = vertices[i].x * sin_r + vertices[i].y * cos_r;

    vertices[i].x = tx + x;
    vertices[i].y = ty + y;

    // Clamp vertex to screen bounds
    clamp_to_screen(&vertices[i]);
  }

  // Draw lines connecting the vertices
  screen_draw_line(color, vertices[0].x, vertices[0].y, vertices[1].x,
                   vertices[1].y);
  screen_draw_line(color, vertices[1].x, vertices[1].y, vertices[2].x,
                   vertices[2].y);
  screen_draw_line(color, vertices[2].x, vertices[2].y, vertices[0].x,
                   vertices[0].y);
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
  wrap_position(&player->position, SCREEN_WIDTH, SCREEN_HEIGHT);
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

      // Render game state
      render_game_state(&state);

      // Swap buffers or refresh the screen
    }
  }
}
