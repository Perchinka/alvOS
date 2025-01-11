#include "../include/asteroids.h"
#include "../include/screen.h"
#include "../include/system.h"
#include "../include/timer.h"

GameObject *asteroids[MAX_ASTEROIDS];
int asteroids_amount = 0;

GameObject *bullets[MAX_BULLETS];
int bullet_count = 0;

// ----- ControlsMap -----

ControlMap controls;

void map_controls(ControlMap *map, InputState *input) {
  map->rotate_left = &input->keys['a'];
  map->rotate_right = &input->keys['d'];
  map->thrust = &input->keys['w'];
  map->shoot = &input->keys[' '];
} // Not sure if it is the best way to do it, but it provides some scaleability

// ----- Sprites (Kinda :D) -----

static void draw_ship(GameObject *player) {
  u8 color = 255;

  float cos_r = cos(player->rotation);
  float sin_r = sin(player->rotation);

  Vector2D transformed[5];
  for (int i = 0; i < 5; i++) {
    float tx = player->vertices[i].x * cos_r - player->vertices[i].y * sin_r;
    float ty = player->vertices[i].x * sin_r + player->vertices[i].y * cos_r;

    transformed[i].x = tx + player->position.x;
    transformed[i].y = ty + player->position.y;
  }

  float x0, y0, x1, y1;

  // Left leg
  x0 = transformed[0].x;
  y0 = transformed[0].y;
  x1 = transformed[1].x;
  y1 = transformed[1].y;
  if (clip_line(&x0, &y0, &x1, &y1))
    screen_draw_line(color, x0, y0, x1, y1);

  // Right leg
  x0 = transformed[0].x;
  y0 = transformed[0].y;
  x1 = transformed[2].x;
  y1 = transformed[2].y;
  if (clip_line(&x0, &y0, &x1, &y1))
    screen_draw_line(color, x0, y0, x1, y1);

  // Bar
  x0 = transformed[3].x;
  y0 = transformed[3].y;
  x1 = transformed[4].x;
  y1 = transformed[4].y;
  if (clip_line(&x0, &y0, &x1, &y1))
    screen_draw_line(color, x0, y0, x1, y1);
}

// I did it this way as I didn't want write whole animation and spriting system
// for GameObjects, so this function will be called in the player render()
// method. Not the best solution definitely, but clean and good enough for now
static void draw_flame(GameObject *player, u8 color) {
  Vector2D flame_vertices[] = {
      {-0.35f, -0.235f}, // Left base
      {-0.35f, 0.235f},  // Right base
      {-0.88f, 0},       // Flame tip
  };
  for (int i = 0; i <= sizeof(flame_vertices) / sizeof(flame_vertices[0]);
       i++) {
    flame_vertices[i] = vector_scale(flame_vertices[i], player->size);
  }

  float cos_r = cos(player->rotation);
  float sin_r = sin(player->rotation);

  // Transform vertices to screen coordinates
  for (int i = 0; i < 3; i++) {
    float tx = flame_vertices[i].x * cos_r - flame_vertices[i].y * sin_r;
    float ty = flame_vertices[i].x * sin_r + flame_vertices[i].y * cos_r;

    flame_vertices[i].x = tx + player->position.x;
    flame_vertices[i].y = ty + player->position.y;
  }

  // Draw lines connecting the vertices with clipping
  for (int i = 0; i < 3; i++) {
    float x0 = flame_vertices[i].x;
    float y0 = flame_vertices[i].y;
    float x1 = flame_vertices[(i + 1) % 3].x;
    float y1 = flame_vertices[(i + 1) % 3].y;

    if (clip_line(&x0, &y0, &x1, &y1)) {
      screen_draw_line(color, x0, y0, x1, y1);
    }
  }
}

// ----- Bullets -----
typedef struct {
  Vector2D position;
  float max_size;
} ExplosionContext;

void explosion_draw_callback(float progress, void *context) {
  ExplosionContext *explosion = (ExplosionContext *)context;
  float max_size = explosion->max_size;

  float current_size = progress * max_size;

  u8 brightness = 255 - (u8)(progress * 255);

  screen_draw_circle(brightness, explosion->position.x, explosion->position.y,
                     current_size);

  float shockwave_size = current_size * 0.8f;
  u8 shockwave_brightness = brightness / 2;
  screen_draw_circle(shockwave_brightness, explosion->position.x,
                     explosion->position.y, shockwave_size);

  if (progress < 0.8f) {
    for (int i = 0; i < 5; i++) {
      float angle = progress * 360 + i * 72;
      float debris_x = explosion->position.x + cos(angle) * current_size;
      float debris_y = explosion->position.y + sin(angle) * current_size;
      screen_draw_line(brightness / 2, explosion->position.x,
                       explosion->position.y, debris_x, debris_y);
    }
  }
}

void spawn_explosion(Vector2D position, float size) {
  ExplosionContext *explosion;
  explosion->position = position;
  explosion->max_size = size;

  run_animation(.5f, explosion_draw_callback, explosion);
}

void update_bullet(GameObject *bullet, float dt) {
  bullet->position =
      vector_add(bullet->position, vector_scale(bullet->velocity, dt));
  wrap_position(&bullet->position, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);

  bullet->time_to_live -= dt;
  if (bullet->time_to_live <= 0.0f) {
    bullet->is_active = false; // Bullet explires
  }

  // Check collision with asteroids
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i]->is_active && check_collision(bullet, asteroids[i])) {
      bullet->is_active = false;
      asteroids[i]->is_active = false;

      spawn_explosion(asteroids[i]->position, asteroids[i]->size);

      break;
    }
  }
}

void render_bullet(GameObject *bullet) {
  if (bullet->is_active) {
    screen_draw_circle(255, bullet->position.x, bullet->position.y,
                       bullet->size);
  }
}

void init_bullet(GameObject *bullet, GameObject *player) {
  bullet->position = player->position;
  bullet->velocity =
      vector_add((Vector2D){cos(player->rotation) * BULLET_SPEED,
                            sin(player->rotation) * BULLET_SPEED},
                 player->velocity);
  bullet->rotation = player->rotation;
  bullet->size = 1;
  bullet->is_active = true;
  bullet->time_to_live = BULLET_LIFESPAN;
  bullet->update = update_bullet;
  bullet->render = render_bullet;
}

// ----- Player Logic (render, update) -----
void render_player(GameObject *player) {
  draw_ship(player);

  static int flame_visible = 0;
  static u32 last_toggle_time = 0;

  u32 current_time = timer_get();
  if (current_time - last_toggle_time > 15) { // Toggle every 15ms
    flame_visible = !flame_visible;
    last_toggle_time = current_time;
  }

  if (controls.thrust->is_down &&
      flame_visible) { // Well, maybe not the best way to do it, but it works :)
    draw_flame(player, 255);
  }
}

void update_player(GameObject *player, float dt) {
  Vector2D direction = {cos(player->rotation), sin(player->rotation)};

  if (controls.rotate_left->is_down) {
    player->rotation -= SHIP_TURN_SPEED * dt;
  }
  if (controls.rotate_right->is_down) {
    player->rotation += SHIP_TURN_SPEED * dt;
  }

  if (player->rotation < 0) {
    player->rotation += 2.0f * PI;
  }
  if (player->rotation >= 2.0f * PI) {
    player->rotation -= 2.0f * PI;
  }

  if (controls.thrust->is_down) {
    Vector2D thrust = {cos(player->rotation), sin(player->rotation)};
    thrust = vector_normalize(thrust);
    thrust = vector_scale(thrust, SHIP_ACCELERATION * dt);
    player->velocity = vector_add(player->velocity, thrust);
  }
  if (controls.shoot->is_pressed) { // Shoot on button press
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!bullets[i]->is_active) { // Find an inactive bullet
        init_bullet(bullets[i], player);
        break;
      }
    }
  }
  // Apply friction to reduce velocity over time
  player->velocity = vector_scale(player->velocity, DAMPNING_FACTOR);

  player->position =
      vector_add(player->position, vector_scale(player->velocity, dt));
  wrap_position(&player->position, SCREEN_WIDTH, SCREEN_HEIGHT, player->size);
}

void init_player(GameObject *player) {
  player->position = (Vector2D){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  player->velocity = (Vector2D){0, 0};
  player->rotation = 0.0f; // Measured in radians
  player->size = 15;       // Collision radius in pixels
  player->is_active = true;

  Vector2D vertices[] = {
      {0.7f, 0},        // Tip
      {-0.59f, 0.47f},  // Base top
      {-0.59f, -0.47f}, // Base bottom
      {-0.35, 0.29},    // Crossbar top
      {-0.35, -0.29},   // Crossbar bottom
  };
  for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); ++i) {
    player->vertices[i] = vector_scale(vertices[i], player->size);
  }

  player->update = update_player;
  player->render = render_player;
}

// ----- Asteroids Logic -----

void update_asteroid(GameObject *asteroid, float dt) {

  asteroid->position =
      vector_add(asteroid->position, vector_scale(asteroid->velocity, dt));
  asteroid->rotation += asteroid->rotation_speed * dt;

  // Keep rotation within 0 to 2π
  if (asteroid->rotation < 0) {
    asteroid->rotation += 2.0f * PI;
  } else if (asteroid->rotation >= 2.0f * PI) {
    asteroid->rotation -= 2.0f * PI;
  }

  wrap_position(&asteroid->position, SCREEN_WIDTH, SCREEN_HEIGHT,
                asteroid->size);
}

void render_asteroid(GameObject *asteroid) {
  float cos_r = cos(asteroid->rotation);
  float sin_r = sin(asteroid->rotation);

  Vector2D transformed[MAX_VERTICES];
  for (int i = 0; i < asteroid->vertex_count; i++) {
    float x = asteroid->vertices[i].x;
    float y = asteroid->vertices[i].y;

    transformed[i].x = x * cos_r - y * sin_r + asteroid->position.x;
    transformed[i].y = x * sin_r + y * cos_r + asteroid->position.y;
  }

  for (int j = 0; j < asteroid->vertex_count; j++) {
    int next = (j + 1) % asteroid->vertex_count;
    float x0 = transformed[j].x;
    float y0 = transformed[j].y;
    float x1 = transformed[next].x;
    float y1 = transformed[next].y;

    if (clip_line(&x0, &y0, &x1, &y1)) {
      screen_draw_line(255, x0, y0, x1, y1);
    }
  }
}

void init_asteroid(GameObject *asteroid) {
  asteroid->position =
      (Vector2D){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};
  do {
    asteroid->velocity =
        (Vector2D){.x = (rand() % 41 - 20.0f), .y = (rand() % 41 - 20.0f)};
  } while (asteroid->velocity.x == 0.0f && asteroid->velocity.y == 0.0f);
  asteroid->rotation = rand() % 360 * (PI / 180.0f);
  asteroid->rotation_speed = (rand() % 3 - 1.5f);

  asteroid->size = 15 + ((rand() % 15));
  asteroid->is_active = true;

  float angle_step = 2.0f * PI / 16; // 16-sided polygon
  for (int i = 0; i < 16; i++) {
    float angle = i * angle_step + (rand() % 10 + 1.0f) * PI / 180.0f;
    float radius = asteroid->size * (0.8f + ((rand() % 5) / 10.0f));
    asteroid->vertices[i].x = cos(angle) * radius;
    asteroid->vertices[i].y = sin(angle) * radius;
  }
  asteroid->vertex_count = 16;

  asteroid->update = update_asteroid;
  asteroid->render = render_asteroid;
}

// ----- Game Loop (and stuff) -----

#define FPS 60

void draw_borders() {
  screen_draw_line(255, 0.0, 0, SCREEN_WIDTH, 0);
  screen_draw_line(255, 0, 0, 0, SCREEN_HEIGHT);
  screen_draw_line(255, SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT);
  screen_draw_line(255, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1);
}

void game_loop() {
  seed(5);
  GameState state;
  init_game_state(&state);
  init_animations();

  GameObject *player = &state.objects[state.object_count++];
  init_player(player);

  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    GameObject *asteroid = &state.objects[state.object_count++];
    init_asteroid(asteroid);
    asteroids[i] = asteroid;
    asteroids_amount += 1;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    GameObject *bullet = &state.objects[state.object_count++];
    init_asteroid(bullet);
    bullets[i] = bullet;
    bullets[i]->is_active = false;
  }

  map_controls(&controls, &state.input);

  u32 last_frame = 0;
  float delta_time = 1.0f / FPS;

  while (true) {
    const u32 now = (u32)timer_get();
    if ((now - last_frame) > (TIMER_TPS / FPS)) {
      last_frame = now;

      screen_clear(0);

      update_input(&state.input);
      update_game_state(&state, delta_time);
      update_animations(delta_time);

      render_game_state(&state);
      draw_borders();

      for (int i = 0; i < asteroids_amount; i++) {
        if (!asteroids[i]->is_active)
          continue;

        if (check_collision(player, asteroids[i])) {
          screen_draw_string("COLLISION", player->position.x,
                             player->position.y, 200);
        }
      }

      // TODO Swap buffers
    }
  }
}
