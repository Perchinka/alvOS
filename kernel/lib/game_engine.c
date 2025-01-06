#include "../include/game_engine.h"

void engine_init(GameEngine *engine) {
  engine->object_count = 0;
  engine->frames = 0;
}

void engine_update(GameEngine *engine, float dt) {
  for (size_t i = 0; i < engine->object_count; i++) {
    GameObject *obj = &engine->objects[i];
    if (obj->active && obj->update) {
      obj->update(obj, dt);
    }
  }
  engine->frames++;
}

void engine_render(GameEngine *engine) {
  for (size_t i = 0; i < engine->object_count; i++) {
    GameObject *obj = &engine->objects[i];
    if (obj->active && obj->render) {
      obj->render(obj, NULL);
    }
  }
}

void engine_add_object(GameEngine *engine, GameObject object) {
  if (engine->object_count < MAX_GAME_OBJECTS) {
    engine->objects[engine->object_count++] = object;
  }
}
