#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "game_engine.h"
#include "utils.h"

#define SHIP_ACCELERATION 200.0f
#define DAMPNING_FACTOR 0.99f

#define SHIP_TURN_SPEED 4.0f
#define SHIP_SIZE 10.0f

typedef struct {
  Control *rotate_left;
  Control *rotate_right;
  Control *thrust;
  Control *shoot;
} ControlMap;

void map_controls(ControlMap *map, InputState *input);

void init_player(GameObject *player);
void update_player(GameObject *player, float dt);

void game_loop();

#endif
