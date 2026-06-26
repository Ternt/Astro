// 2026-06-17
//
// TODO list (features, improvements, and bug fixes)
// -------------------------------------------------
// [ ] Resize window when resizing occurs in the 
//     browser. Currently window sizing is set 
//     only on startup. 
// [ ] 

////////////////////////////
//- Third Party Includes

#if !defined(PLATFORM_WEB)
#include "raylib/external/glad.h"
#else
#include <GLES3/gl3.h>
#endif
#include "raylib/raylib.h"
#include "raylib/raymath.h"
#include "raylib/rlgl.h"

////////////////////////////
//- Game Configurations

// profiling macros
#if !defined(PLATFORM_WEB)
#define PROFILE_TRACY 1
#endif

// opengl version
#if defined(PLATFORM_WEB)
# ifndef GRAPHICS_API_OPENGL_ES3
# define GRAPHICS_API_OPENGL_ES3 1
# endif
#else
# ifndef GRAPHICS_API_OPENGL_33
# define GRAPHICS_API_OPENGL_33 1
# endif
#endif

////////////////////////////
//- Game Includes

#include "helpers.h"
#include "profiling.h"
#include "arena.h"
#include "render.h"
#include "geometry.h"
#include "draw.h"
#include "game.h"

#include "helpers.c"
#include "arena.c"
#include "render.c"
#include "geometry.c"
#include "draw.c"
#include "game.c"

////////////////////////////
//- Game Globals

f32           g_worldBoundX = 1000.0f;
f32           g_worldBoundY = 1000.0f;
GLuint        g_astroid_hexBuffer = 0;
Mesh2D        g_astroid_hexShape = zero_struct;
Xform*        g_astroid_hexXforms = null;
u32           g_astroid_hexInstCount = 1024*100;
R_Hull2DInst* g_astroid_hexInst = null;
R_PassArray*  g_passes = null;

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
          u32 inst_cap = g_astroid_hexInstCount * (MB(2)/(g_astroid_hexInstCount * sizeof(R_Hull2DInst)));
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
  {
    Color fpsColor = LIME;
    u32   fps = GetFPS();
    if (InRange(fps, 15, 30)) fpsColor = ORANGE;
    if (InRange(fps,  0, 14)) fpsColor = RED;
    DrawTextEx(GAME.default_font, TextFormat("%2i FPS", fps), (Vector2){0, 0}, 20, 1, fpsColor);
    DrawTextEx(GAME.default_font, TextFormat("zoom %.3f\n", GAME.camera_zoom), (Vector2){0, 20}, 20, 1, WHITE);
    DrawTextEx(GAME.default_font, TextFormat("count %d\n", g_astroid_hexInstCount), (Vector2){0, 40}, 20, 1, WHITE);

    // Arena Memory Usage UI
    Game_DebugArena();
  }

  // World-space debug
  {
    BeginMode2D(GAME.camera_2d);
    DR_DrawRectangleLines(-g_worldBoundX/2.0f, 
        -g_worldBoundY/2.0f,
        g_worldBoundX, 
        g_worldBoundY, 
        LIME);
    EndMode2D();
  }

  ProfEnd();
}

////////////////////////////
//- Game Entry Point (Game-specific Initializations)

static void Game_EntryPoint(int argc, char *argv[])
{
  // initialize per-instance randomized transforms for astroids
  g_astroid_hexShape = Geo_GenMesh2DConvex(8);
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

