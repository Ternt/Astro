// 2026-06-25

/////////////////////////////
//- Physics2D Includes

#include "physics/physics_core.c"
#include "physics/physics_broadphase.c"

/////////////////////////////
//- Physics2D Functions

static void P2_StepWorld(P2_WorldId world_id, f32 delta)
{
  ProfBegin("P2_StepWorld");
  P2_WorldData *world = P2_GetWorldData(world_id);
  for(P2_BodyData *body = world->first_body; body != null; body = body->next)
  {
    body->vel = Vector2Add(body->vel, Vector2Scale(body->acc, delta));
    body->pos = Vector2Add(body->pos, Vector2Scale(body->vel, delta));

    body->angular_vel += body->angular_acc * delta;
    body->rot += body->angular_vel * delta;
  }
  ProfEnd();
}

static void P2_Init(void)
{
  Arena *arena = ArenaAlloc("physics", KB(8), false);
  P2 = PushArray(arena, P2_Ctx, 1);
  P2->arena = arena;
  P2->worlds = PushArray(arena, P2_WorldData, P2_MAX_WORLDS);
  P2->bodies_arena = ArenaAlloc("physics_bodies", KB(64), true);
  P2->aabb_tree_arena = ArenaAlloc("physics_aabb_tree", KB(64), true);
}

static void P2_Quit(void)
{
  ArenaRelease(P2->bodies_arena);
  ArenaRelease(P2->arena);
}
