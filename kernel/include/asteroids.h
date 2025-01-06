#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "game_engine.h"
#include "utils.h"

#define SHIP_ACCELERATION .4f
#define DAMPNING_FACTOR 0.985f

#define SHIP_TURN_SPEED 0.1f
#define SHIP_SIZE 10.0f

#define NUM_CONTROLS 4

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

void update_game_state();
void render_game_state();
#endif
