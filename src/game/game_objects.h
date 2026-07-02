// 2026-07-01

#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

/////////////////////////////
//- GameObject Parameters

typedef struct GameObjectParams GameObjectParams;
struct GameObjectParams {
  Vector3 translation;
  F32 rotation;
  Vector3 scale;
};

/////////////////////////////
//- GameObject & Data Structure Typedef

typedef struct GameObject GameObject;
struct GameObject {
  String8 name;
  Vector2 translation;
  F32 rotation;
  Vector2 scale;
};

typedef struct GameObjectNode GameObjectNode;
struct GameObjectNode {
  GameObjectNode *next;
  GameObjectNode *prev;
  GameObject v;
};

typedef struct GameObjectMap GameObjectMap;
struct GameObjectMap {
  GameObjectNode **slots;
  U64 slot_count;
};

/////////////////////////////
//- GameObject Default Paramter Values

const Vector2 game_default_translation = {0.f, 0.f};
const F32 game_default_rotation = 0.f;
const Vector2 game_default_scale = {1.f, 1.f};

/////////////////////////////
//- GameObject Functions

#define GameObject_Make(...) GameObject_Make_(&(GameObjectParams){ .translation = game_default_translation, .rotation = game_default_rotation, .scale = game_default_scale, __VA_ARGS__ })
static GameObject  GameObject_Make_(GameObjectParams *params);

static GameObject* GameObjectMap_Insert(Arena *arena, GameObjectMap *map, String8 key);
static void        GameObjectMap_Remove(GameObjectMap *map, String8 key);
static GameObject* GameObjectMap_Get(GameObjectMap *map, String8 key);

#endif // GAME_OBJECTS_H
