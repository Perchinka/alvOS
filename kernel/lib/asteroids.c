#include "../include/asteroids.h"
#include "../include/screen.h"
#include "../include/system.h"
#include "../include/timer.h"

GameState game_state;
GameObject *player;

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

// ----- Animations -----

typedef struct {
  Vector2D position;
  float max_size;
} ExplosionContext;

void explosion_draw_callback(float progress, void *context) {
  ExplosionContext *explosion = (ExplosionContext *)context;
  float max_size = explosion->max_size;

  float current_size = progress * max_size;

  u8 brightness = 255 - (u8)(progress * 255);

  // Draw the outer explosion circle
  float x = explosion->position.x;
  float y = explosion->position.y;
  if (clip_circle(x, y, current_size)) {
    screen_draw_circle(brightness, x, y, current_size);
  }

  // Draw the shockwave
  float shockwave_size = current_size * 0.8f;
  u8 shockwave_brightness = brightness / 2;
  if (clip_circle(x, y, shockwave_size)) {
    screen_draw_circle(shockwave_brightness, x, y, shockwave_size);
  }

  // Add debris lines
  if (progress < 0.8f) {
    for (int i = 0; i < 5; i++) {
      float angle = progress * 360 + i * 72;
      float debris_x = x + cos(angle) * current_size;
      float debris_y = y + sin(angle) * current_size;

      float debris_start_x = x;
      float debris_start_y = y;

      if (clip_line(&debris_start_x, &debris_start_y, &debris_x, &debris_y)) {
        screen_draw_line(brightness / 2, debris_start_x, debris_start_y,
                         debris_x, debris_y);
      }
    }
  }
}

void spawn_explosion(Vector2D position, float size) {
  ExplosionContext *explosion;
  explosion->position = position;
  explosion->max_size = size;

  run_animation(.5f, explosion_draw_callback, explosion);
}

// ----- Asteroids Logic -----

void clear_asteroids() {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i]) {
      asteroids[i]->is_active = false;
    }
  }
  asteroids_amount = 0;
}

void update_asteroid(GameObject *asteroid, float dt) {
  if (!asteroid->is_active)
    return;

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
  if (!asteroid->is_active)
    return;

  float cos_r = cos(asteroid->rotation);
  float sin_r = sin(asteroid->rotation);

  Vector2D transformed[MAX_VERTICES];
  for (int i = 0; i < asteroid->vertex_count; i++) {
    // Scale vertices by size and rotate them
    float x = asteroid->vertices[i].x * asteroid->size;
    float y = asteroid->vertices[i].y * asteroid->size;

    // Apply rotation and translation
    transformed[i].x = x * cos_r - y * sin_r + asteroid->position.x;
    transformed[i].y = x * sin_r + y * cos_r + asteroid->position.y;
  }

  // Draw lines between consecutive vertices
  for (int j = 0; j < asteroid->vertex_count; j++) {
    int next =
        (j + 1) % asteroid->vertex_count; // Wrap around to the first vertex
    float x0 = transformed[j].x;
    float y0 = transformed[j].y;
    float x1 = transformed[next].x;
    float y1 = transformed[next].y;

    if (clip_line(&x0, &y0, &x1, &y1)) {
      screen_draw_line(255, x0, y0, x1, y1);
    }
  }
}

GameObject *_init_asteroid(GameObject *asteroid) {
  if (asteroids_amount >= MAX_ASTEROIDS) {
    // Find an inactive asteroid to reuse
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
      if (!asteroids[i]->is_active) {
        asteroid = asteroids[i];
        break;
      }
    }
    // If no inactive asteroids available - exit
    if (asteroid == NULL)
      return NULL;
  } else {
    // Allocate a new asteroid
    asteroid = &game_state.objects[game_state.object_count++];
  }

  asteroid->size = 15 + (rand() % 15);
  float min_distance = 20.0f;
  do {
    asteroid->position =
        (Vector2D){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};
  } while (vector_length(vector_sub(player->position, asteroid->position)) <
           min_distance + asteroid->size);

  do {
    asteroid->velocity = (Vector2D){rand() % 41 - 20.0f, rand() % 41 - 20.0f};
  } while (asteroid->velocity.x == 0.0f && asteroid->velocity.y == 0.0f);

  asteroid->rotation = rand() % 360 * (PI / 180.0f);
  asteroid->rotation_speed = (rand() % 3 - 1.5f);

  asteroid->is_active = true;

  // I generate a unit-circle polygon, that after that in
  // render will be scaled by size factor
  float angle_step = 2.0f * PI / 16; // 16-sided polygon
  for (int i = 0; i < 16; i++) {
    float angle = i * angle_step + (rand() % 10 + 1.0f) * PI / 180.0f;
    float radius = 1.0f * (0.8f + ((rand() % 5) / 10.0f));
    asteroid->vertices[i].x = cos(angle) * radius;
    asteroid->vertices[i].y = sin(angle) * radius;
  }

  asteroid->vertex_count = 16;
  asteroid->update = update_asteroid;
  asteroid->render = render_asteroid;

  if (asteroids_amount < MAX_ASTEROIDS &&
      asteroids[asteroids_amount] != asteroid) {
    asteroids[asteroids_amount++] = asteroid;
  }

  return asteroid;
}

void remove_asteroid(int index) {
  if (index < 0 || index >= asteroids_amount)
    return;

  // Award points based on asteroid size
  const float max_size = 30.0f;
  const int base_points = 5;
  float asteroid_size = asteroids[index]->size;

  game_state.score += (int)(base_points * (max_size / asteroid_size));

  spawn_explosion(asteroids[index]->position, asteroids[index]->size);
  asteroids[index]->is_active = false;

  // Shift remaining asteroids to fill the gap
  for (int i = index; i < asteroids_amount - 1; i++) {
    asteroids[i] = asteroids[i + 1];
  }

  asteroids_amount--;
}

int find_asteroid_index(GameObject *asteroid) {
  for (int i = 0; i < asteroids_amount; i++) {
    if (asteroids[i] == asteroid)
      return i;
  }
  return -1;
}

void split_asteroid(GameObject *asteroid) {
  float size_limit = 10.0f;

  if (asteroid->size <= size_limit) {
    remove_asteroid(find_asteroid_index(asteroid)); // Remove if too small
    return;
  }

  float size = asteroid->size;
  Vector2D position = asteroid->position;

  remove_asteroid(find_asteroid_index(asteroid)); // Remove original asteroid

  int max_fragments = size / size_limit;
  int fragment_count = 2.0f + rand() % max_fragments;
  float new_size = MAX(size / fragment_count, 4.0f);

  for (int i = 0; i < fragment_count; i++) {
    GameObject *fragment = _init_asteroid(NULL);
    if (fragment == NULL)
      break; // Stop if asteroids full

    fragment->position = (Vector2D){
        position.x - (size / 2) + rand() % (int)new_size,
        position.y - (size / 2) + rand() % (int)new_size,
    };
    fragment->size = new_size;
    vector_scale(fragment->velocity, 1 + rand() % 80 * 0.01f);
  }
}

// ----- Bullets -----
void update_bullet(GameObject *bullet, float dt) {
  if (!bullet->is_active)
    return;

  bullet->position =
      vector_add(bullet->position, vector_scale(bullet->velocity, dt));
  wrap_position(&bullet->position, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);

  bullet->time_to_live -= dt;
  if (bullet->time_to_live <= 0.0f) {
    bullet->is_active = false;
    return;
  }

  // Check for collisions
  for (int i = 0; i < asteroids_amount; i++) {
    if (asteroids[i]->is_active && check_collision(bullet, asteroids[i])) {
      bullet->is_active = false;
      split_asteroid(asteroids[i]);
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

void render_menu() {
  screen_clear(0);

  const char *title = "ASTEROIDS OS";
  int title_x = SCREEN_WIDTH / 2 - (strlen(title) * 4);
  int title_y = SCREEN_HEIGHT / 3;
  screen_draw_string(title, title_x, title_y, 255);

  const char *instruction = "Press Enter to Start";
  int instruction_x = SCREEN_WIDTH / 2 - (strlen(instruction) * 4);
  int instruction_y = SCREEN_HEIGHT / 2;
  screen_draw_string(instruction, instruction_x, instruction_y, 255);
}

// ----- Game Loop (and stuff) -----

#define FPS 60

void draw_borders() {
  screen_draw_line(255, 0.0, 0, SCREEN_WIDTH, 0);
  screen_draw_line(255, 0, 0, 0, SCREEN_HEIGHT);
  screen_draw_line(255, SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT);
  screen_draw_line(255, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1);
}

void start_new_game() {
  init_game_state(&game_state);
  clear_asteroids();

  player = &game_state.objects[game_state.object_count++];
  init_player(player);

  for (int i = 0; i < 10; i++) {
    init_asteroid();
  }

  // Initialize bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    GameObject *bullet = &game_state.objects[game_state.object_count++];
    init_bullet(bullet, player);
    bullets[i] = bullet;
    bullets[i]->is_active = false;
  }
}

void render_game_over() {
  const char *game_over_text = "GAME OVER";
  const char *restart_text = "Press Enter to Return to Menu";

  int game_over_x = SCREEN_WIDTH / 2 - (strlen(game_over_text) * 4);
  int game_over_y = SCREEN_HEIGHT / 3;
  screen_draw_string(game_over_text, game_over_x, game_over_y, 255);

  int restart_x = SCREEN_WIDTH / 2 - (strlen(restart_text) * 4);
  int restart_y = SCREEN_HEIGHT / 2;
  screen_draw_string(restart_text, restart_x, restart_y, 255);
}

static void draw_lives(int amount, u8 color) {
  const float icon_size = .7f;
  const int spacing = 14;
  const int start_x = SCREEN_WIDTH - (amount * spacing) - 10;
  const int start_y = 15;

  const float cos_r = 0.0f;
  const float sin_r = -1.0f;

  for (int i = 0; i < amount; i++) {
    float offset_x = start_x + i * spacing;
    float offset_y = start_y;

    Vector2D transformed[5];

    // Transform each vertex of the ship
    for (int j = 0; j < 5; j++) {
      // Scale the vertex
      float scaled_x = player->vertices[j].x * icon_size;
      float scaled_y = player->vertices[j].y * icon_size;

      // Rotate 90 degrees to the left (counterclockwise)
      transformed[j].x = scaled_x * cos_r - scaled_y * sin_r + offset_x;
      transformed[j].y = scaled_x * sin_r + scaled_y * cos_r + offset_y;
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
}

void update_ui() {
  char score_text[50];
  itoa(game_state.score, score_text);
  screen_draw_string(score_text, 10, 10, 188);
  draw_lives(game_state.lives, 148);
}

void check_collisions() {
  for (int i = 0; i < asteroids_amount; i++) {
    if (asteroids[i]->is_active && check_collision(player, asteroids[i])) {
      game_state.lives--;

      // Calculate bump
      Vector2D collision_normal =
          vector_sub(player->position, asteroids[i]->position);
      collision_normal = vector_normalize(collision_normal);

      // Reflect velocity
      float dot_product = vector_dot(player->velocity, collision_normal);
      player->velocity = vector_sub(
          player->velocity, vector_scale(collision_normal, 1.0f * dot_product));

      // Apply bump
      player->position =
          vector_add(player->position,
                     vector_scale(collision_normal, asteroids[i]->size * 0.3f));

      spawn_explosion(player->position, player->size);
      split_asteroid(asteroids[i]);

      if (game_state.lives <= 0) {
        game_state.is_game_over = true;
      }
    }
  }
}

void game_loop() {
  seed(5);
  init_animations();

  game_state.in_menu = true;
  game_state.is_game_over = false;

  map_controls(&controls, &game_state.input);

  u32 last_frame = 0;
  float delta_time = 1.0f / FPS;
  bool enter_released = true;

  while (true) {
    const u32 now = (u32)timer_get();
    if ((now - last_frame) > (TIMER_TPS / FPS)) {
      last_frame = now;

      screen_clear(0);

      update_input(&game_state.input);

      // Menu logic
      if (game_state.in_menu) {
        render_menu();
        if (KEYBOARD_KEY(KEY_ENTER) && enter_released) {
          game_state.in_menu = false;
          enter_released = false;
          start_new_game();
        }

        if (!KEYBOARD_KEY(KEY_ENTER)) {
          enter_released = true;
        }

        continue;
      }

      // Game logic
      if (!game_state.is_game_over) {
        update_game_state(&game_state, delta_time);
        check_collisions();

        update_animations(delta_time);
        render_game_state(&game_state);
        update_ui();
      } else {
        render_game_over();
        if (KEYBOARD_KEY(KEY_ENTER) && enter_released) {
          game_state.in_menu = true;
          game_state.is_game_over = false;
          enter_released = false;
        }
        if (!KEYBOARD_KEY(KEY_ENTER)) {
          enter_released = true;
        }
      }

      draw_borders();

      // TODO: Swap buffers
    }
  }
}
