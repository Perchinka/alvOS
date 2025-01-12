#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "game_engine.h"
#include "utils.h"

#define SHIP_ACCELERATION 200.0f
#define DAMPNING_FACTOR 0.99f

#define SHIP_TURN_SPEED 5.0f
#define SHIP_SIZE 10.0f

#define MAX_ASTEROIDS 20
#define MAX_BULLETS 20
#define BULLET_SPEED 300.0f
#define BULLET_LIFESPAN 0.8f

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define init_asteroid(...)                                                     \
  GET_MACRO(__VA_ARGS__, init_asteroid_with_size, init_asteroid_default)       \
  (__VA_ARGS__)

#define init_asteroid_with_size(asteroid) _init_asteroid(asteroid)

#define init_asteroid_default() _init_asteroid(NULL)

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
