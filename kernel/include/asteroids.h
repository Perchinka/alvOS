#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#define MAX_SHIP_SPEED 4.0f
#define SHIP_ACCELERATION 1.0f
#define SHIP_TURN_SPEED 0.1f
#define SHIP_SIZE 10.0f

typedef struct {
  float x, y;   // Position
  float angle;  // Rotation angle in radians
  float vx, vy; // Velocity
} Ship;

void update_game_state();
void render_game_state();
#endif
