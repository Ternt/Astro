// 2026-06-28

////////////////////////////
//- Build Macro Configs

// profiling macros
#if !defined(PLATFORM_WEB) && defined(BUILD_DEBUG)
#define PROFILE_TRACY 1
#endif

////////////////////////////
//- Game Macro Configs

#define GAME_ARENA_SIZE KB(64)
#define R_DEFAULT_OGL_BUFFER_SIZE MB(64)

////////////////////////////
//- Game Includes

//- [h]
#include "game/game.h"

//- [c]
#include "game/game.c"

////////////////////////////
//- Game Globals

f32           g_worldBoundX = 100.0f;
f32           g_worldBoundY = 100.0f;
R_PassArray*  g_passes = null;
P2_WorldId    g_world_id = null_id;
u32           g_astroid_hexInstCount = 2;
GLuint        g_astroid_hexBuffer = 0;
Mesh2D        g_astroid_hexShape = zero_struct;
Xform*        g_astroid_hexXforms = null;
R_Hull2DInst* g_astroid_hexInst = null;
P2_BodyId*    g_astroid_hexBodyIds = null;

////////////////////////////
//- Game Loop Pipeline

static inline void 
Game_UpdatePhysics(void)
{
  ProfBegin("Game_UpdatePhysics");
  P2_StepWorld(g_world_id, GetFrameTime());
  ProfEnd();
}

static inline void 
Game_UpdateState(void)
{
  ProfBegin("Game_UpdateState");
  Game_HandleDebugControls();
  Game_HandleCameraControls();

  // Build rendering pipeline
  {
    ProfBegin("Push Instances");
    g_passes = R_AllocPassArray(GAME.arena);

    // geo2d pass
    R_Pass *pass = R_PushPass(g_passes, R_PassType_Geo2D);
    {
      // Initialize Geo2D Params
      int w = GetScreenWidth();
      int h = GetScreenHeight();
      pass->params_geo_2d.view = GetCameraMatrix2D(GAME.camera_2d);
      pass->params_geo_2d.proj = MatrixOrtho(0, w, h, 0, 0.0f, 10.0f);

      R_BatchGroup2DList *batch_groups = &pass->params_geo_2d.batch_groups;

      // Initialize a 2D batch group for hexagonal instances
      {
        // Allocate a batch group
        R_BatchGroup2DNode *node = PushArray(GAME.arena, R_BatchGroup2DNode, 1);
        QueuePush(batch_groups->first, batch_groups->last, node);
        batch_groups->node_count += 1;

        // Allocate static mesh vertices buffer.
        u32 size = g_astroid_hexShape.vert_count * sizeof(Vector2);
        g_astroid_hexBuffer = R_AllocStaticBuffer(g_astroid_hexBuffer, size, g_astroid_hexShape.data);
        node->params.mesh_vertices = g_astroid_hexBuffer;
        node->params.vert_count = g_astroid_hexShape.vert_count;

        // Initialize group with N instances
        node->batches = R_MakeBatchList(sizeof(R_Hull2DInst));
        for(u32 i = 0; i < g_astroid_hexInstCount; i += 1)
        {
          u32 inst_cap = Min(g_astroid_hexInstCount, R_DEFAULT_OGL_BUFFER_SIZE/sizeof(R_Hull2DInst));
          R_Hull2DInst *hull_inst = (R_Hull2DInst*)R_PushBatchInst(GAME.arena, &node->batches, inst_cap);
          hull_inst->tr = P2_GetBodyPosition(g_astroid_hexBodyIds[i]);
          hull_inst->rt = P2_GetBodyRotation(g_astroid_hexBodyIds[i]);
          hull_inst->sc = g_astroid_hexXforms[i].sc;
        }
      }
    }
    ProfEnd();
  }

  ProfEnd();
}

static inline void 
Game_DrawWorld(void)
{
  R_Pass *pass = &g_passes->v[R_PassType_Geo2D];
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  pass->params_geo_2d.view = GetCameraMatrix2D(GAME.camera_2d);
  pass->params_geo_2d.proj = MatrixOrtho(0, w, h, 0, 0.0f, 10.0f);

  ProfBegin("R_DrawAll");
  R_DrawAll(g_passes);
  ProfEnd();
}

static void
Game_DrawUI(void)
{
}

static void
Game_DrawDebug(void)
{
  ProfBegin("Debug UI");

  // Screen-space debug
  Color fps_color = LIME;
  u32 fps = GetFPS();
  if(InRange(fps, 15, 30)) fps_color = ORANGE;
  if(InRange(fps, 0, 14)) fps_color = RED;
  DR_DrawTextEx(TextFormat("%2i FPS", fps), 0, 0, 20, fps_color);
  DR_DrawTextEx(TextFormat("zoom %.3f\n", GAME.camera_zoom), 0, 20, 20, WHITE);
  DR_DrawTextEx(TextFormat("count %d\n", g_astroid_hexInstCount), 0, 40, 20, WHITE);
  Game_DebugArena();

  // World-space debug
  BeginMode2D(GAME.camera_2d);
  DR_DrawRectangleLines(-g_worldBoundX/2.0f, -g_worldBoundY/2.0f, g_worldBoundX, g_worldBoundY, LIME);
  EndMode2D();

  ProfEnd();
}

////////////////////////////
//- Game Entry Point (Game-specific Initializations)

static void Game_EntryPoint(int argc, char *argv[])
{
  P2_WorldParams world_params = zero_struct;
  world_params.bounds = (Vector2){g_worldBoundX, g_worldBoundY};
  world_params.gravity = 9.8f;
  g_world_id = P2_CreateWorld(&world_params);

  // initialize per-instance randomized transforms for astroids
  g_astroid_hexShape = Geo_GenMesh2DConvex(GAME.vertex_arena, 6);
  g_astroid_hexXforms = PushArray(GAME.arena, Xform, g_astroid_hexInstCount);
  g_astroid_hexBodyIds = PushArray(GAME.arena, P2_BodyId, g_astroid_hexInstCount);
  for(u32 i = 0; i < g_astroid_hexInstCount; i += 1)
  {

    // Create Game Object
    {
      f32 randScale = RandF32(1.0f, 2.5f);
      f32 randRot   = RandF32(0.0, 360.0);
      f32 randPosX  = RandF32(-0.1*g_worldBoundX, 0.1*g_worldBoundX);
      f32 randPosY  = RandF32(-0.1*g_worldBoundY, 0.1*g_worldBoundY);
      f32 randAccX  = RandF32(-1.0, 1.0);
      f32 randAccY  = RandF32(-1.0, 1.0);
      f32 randAngularAcc = RandF32(-0.5, 0.5);

      g_astroid_hexXforms[i].sc = (Vector2){randScale, randScale};
      g_astroid_hexXforms[i].tr = (Vector2){randPosX, randPosY};
      g_astroid_hexXforms[i].rt = randRot;

      P2_BodyParams body_params = zero_struct;
      body_params.pos = (Vector2){randPosX, randPosY};
      body_params.acc = (Vector2){0.0, 0.0};
      body_params.rot = randRot;
      body_params.angular_acc = 0.0f;
      body_params.mass = 1.0f;

      g_astroid_hexBodyIds[i] = P2_CreateBody(g_world_id, &body_params);
    }

  }
}
