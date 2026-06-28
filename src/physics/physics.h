// 2026-06-25

#ifndef PHYSICS_H
#define PHYSICS_H

/////////////////////////////
//- Physics2D Includes

#include "physics/physics_core.h"
#include "physics/physics_broadphase.h"

/////////////////////////////
//- Physics2D Context

typedef struct P2_Ctx {
  Arena *arena;
  Arena *bodies_arena;
  Arena *aabb_tree_arena;
  P2_WorldData *worlds;
  u32 world_count;
  u32 total_body_count;
} P2_Ctx;

/////////////////////////////
//- Physics2D Globals

static P2_Ctx *P2 = null;

/////////////////////////////
//- Physics2D Functions

static void P2_StepWorld(P2_WorldId world_id, f32 delta);
static void P2_Init(void);
static void P2_Quit(void);

#endif // PHYSICS_H
