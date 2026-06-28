// 2026-06-17
//
// TODO list (features, improvements, and bug fixes)
// -------------------------------------------------
// [ ] Resize window when resizing occurs in the 
//     browser. Currently window sizing is set 
//     only on startup. 
// [ ] A mesh map where each entry corresponds to a 
//     mesh. Each mesh contains a unique number of 
//     vertices.

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

f32           g_worldBoundX = 10000.0f;
f32           g_worldBoundY = 10000.0f;
R_PassArray*  g_passes = null;

u32           g_astroid_hexInstCount = 1024 * 1000;
GLuint        g_astroid_hexBuffer = 0;
Mesh2D        g_astroid_hexShape = zero_struct;
Xform*        g_astroid_hexXforms = null;
R_Hull2DInst* g_astroid_hexInst = null;

////////////////////////////
//- Game Loop Pipeline

static inline void 
Game_UpdatePhysics(void)
{
  ProfBegin("Game_UpdatePhysics");
  ProfEnd();
}

static inline void 
Game_UpdateState(void)
{
  ProfBegin("Game_UpdateState");
  Game_HandleCameraControls();
  Game_HandleDebugControls();

  // build rendering pipeline
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
          u32 inst_cap = R_DEFAULT_OGL_BUFFER_SIZE/sizeof(R_Hull2DInst);
          R_Hull2DInst *hull_inst = (R_Hull2DInst*)R_PushBatchInst(GAME.arena, &node->batches, inst_cap);
          hull_inst->tr = g_astroid_hexXforms[i].tr;
          hull_inst->rt = g_astroid_hexXforms[i].rt;
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
  // initialize per-instance randomized transforms for astroids
  g_astroid_hexShape = Geo_GenMesh2DConvex(GAME.vertex_arena, 8);
  g_astroid_hexXforms = PushArray(GAME.arena, Xform, g_astroid_hexInstCount);
  for(u32 i = 0; i < g_astroid_hexInstCount; i += 1)
  {
    f32 randScale = RandF32(1.0f, 2.5f);
    f32 randPosX  = RandF32(-0.5*g_worldBoundX, 0.5*g_worldBoundX);
    f32 randPosY  = RandF32(-0.5*g_worldBoundY, 0.5*g_worldBoundY);

    g_astroid_hexXforms[i].sc = (Vector2){randScale, randScale};
    g_astroid_hexXforms[i].tr = (Vector2){randPosX, randPosY};
  }
}
