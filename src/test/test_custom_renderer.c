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
// #if !defined(PLATFORM_WEB)
// #define PROFILE_TRACY 1
// #endif

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
#include "entity.h"
#include "game.h"

#include "helpers.c"
#include "arena.c"
#include "render.c"
#include "geometry.c"
#include "draw.c"
#include "entity.c"
#include "game.c"

////////////////////////////
//- Game Globals

GLuint      g_astroid_hexBuffer = 0;
NGon        g_astroid_hexShape = zero_struct;
Xform*      g_astroid_hexXforms = null;
u32         g_astroid_hexInstCount = 32;
R_Hull2DInst *g_astroid_hexInst = null;

GLuint      g_playerBuffer = 0;
NGon        g_playerShape = zero_struct;
Xform*      g_playerXform = null;
f32         g_playerMoveSpeed = 1.0f/75.0f;
f32         g_playerTurnSpeed = 1.0f/15.0f;
R_Hull2DInst *g_playerInst = null;

f32         g_worldBoundX = 200.0f;
f32         g_worldBoundY = 200.0f;

R_PassArray g_passes = zero_struct;

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

  // Get current transforms
  Xform xform = *g_playerXform;
  Vector2 tr = xform.tr;
  f32     rt = xform.rt;
  Vector2 sc = xform.sc;

  Direction dir = Direction_Null;
  f32 angle = rt;
  if(IsKeyDown(KEY_W))
  {
    dir = Direction_North;
  }
  if(IsKeyDown(KEY_S))
  {
    dir = Direction_South;
  }
  if(IsKeyDown(KEY_E))
  {
    angle += g_playerTurnSpeed*DEG2RAD;
  }
  if(IsKeyDown(KEY_Q))
  {
    angle -= g_playerTurnSpeed*DEG2RAD;
  }

  // Calculate orientation vector
  Vector2 axis = {0.0, 1.0};
  Vector2 orientation = Vector2Rotate(axis, angle);
  Vector2 disp = Vector2Zero();
  switch(dir)
  {
    default:break;
    case Direction_North:{disp = Vector2Scale(Vector2Normalize(orientation),  1.0f);}break;
    case Direction_South:{disp = Vector2Scale(Vector2Normalize(orientation), -1.0f);}break;
  }
  disp = Vector2Scale(disp, g_playerMoveSpeed);
  disp = Vector2Add(tr, disp);

  // Update player transformation state
  g_playerXform->tr = disp;
  g_playerXform->rt = angle;

  // Get updated transforms
  tr = xform.tr;
  rt = xform.rt;
  sc = xform.sc;

  // Update instance state
  Matrix model = MatrixIdentity();
  model = MatrixMultiply(model, MatrixScale(sc.x, sc.y, 1.0f));
  model = MatrixMultiply(model, MatrixRotate((Vector3){0.0f, 0.0f, 1.0f}, rt));
  model = MatrixMultiply(model, MatrixTranslate(tr.x, tr.y, 0.0f));
  g_playerInst->model = MatrixTranspose(model);

  // Update camera state
  GAME.camera_2d.target = tr;

  Game_HandleCameraControls();
  {
    ProfBegin("Update Tree");
    ProfEnd();
  }
  ProfEnd();
}

static inline void 
Game_DrawWorld(void)
{
  R_Pass *pass = &g_passes.v[R_PassType_Geo2D];
  {
    // Initialize Geo2D Params
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    pass->params_geo_2d.view = GetCameraMatrix2D(GAME.camera_2d);
    pass->params_geo_2d.proj = MatrixOrtho(0, w, h, 0, 0.0f, 10.0f);
  }

  ProfBegin("R_DrawAll");
  R_DrawAll(&g_passes);
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
  Color fpsColor = LIME;
  u32   fps = GetFPS();
  if (InRange(fps, 15, 30)) fpsColor = ORANGE;
  if (InRange(fps,  0, 14)) fpsColor = RED;
  DrawTextEx(GAME.default_font, TextFormat("%2i FPS", fps), (Vector2){0, 0}, 20, 1, fpsColor);
  DrawTextEx(GAME.default_font, TextFormat("zoom %.3f\n", GAME.camera_zoom), (Vector2){0, 20}, 20, 1, WHITE);
  DrawTextEx(GAME.default_font, TextFormat("count %d\n", g_astroid_hexInstCount), (Vector2){0, 40}, 20, 1, WHITE);

  // World-space debug
  BeginMode2D(GAME.camera_2d);
    DR_DrawRectangleLines(-g_worldBoundX/2.0f, 
                          -g_worldBoundY/2.0f,
                           g_worldBoundX, 
                           g_worldBoundY, 
                           LIME);

    // Get current transforms
    Xform xform = *g_playerXform;
    Vector2 tr = xform.tr;
    f32     rt = xform.rt;
    Vector2 sc = xform.sc;

    // Draw Line
    Vector2 start_pos = {tr.x, tr.y};
    Vector2 axis = {0.0, 1.0};
    Vector2 dir = Vector2Rotate(axis, rt);
    DR_DrawLine2D(start_pos, dir, 1.5f, RED);
  EndMode2D();

  ProfEnd();
}

////////////////////////////
//- Game Entry Point (Game-specific Initializations)

static void Game_EntryPoint(int argc, char *argv[])
{
  // initialize player spaceship
  g_playerShape = Geo_GenerateNGonConvex(GAME.arena, 3);
  g_playerXform = PushArray(GAME.arena, Xform, 1);
  g_playerXform->tr = Vector2Zero();
  g_playerXform->rt = 0.0f;
  g_playerXform->sc = Vector2One();

  // initialize per-instance randomized transforms for astroids
  g_astroid_hexShape = Geo_GenerateNGonConvex(GAME.arena, 8);
  g_astroid_hexXforms = PushArray(GAME.arena, Xform, g_astroid_hexInstCount);
  for(u32 i = 0; i < g_astroid_hexInstCount; i += 1)
  {
    f32 randScale = RandF32(1.0f, 2.5f);
    g_astroid_hexXforms[i].sc = (Vector2){randScale, randScale};

    f32 randPosX = RandF32(-0.5*g_worldBoundX, 0.5*g_worldBoundX);
    f32 randPosY = RandF32(-0.5*g_worldBoundY, 0.5*g_worldBoundY);
    g_astroid_hexXforms[i].tr = (Vector2){randPosX, randPosY};
  }

  // build rendering pipeline
  {
    ProfBegin("Push Instances");

    g_passes = R_MakePassArray();

    // geo2d pass
    R_Pass *pass = R_PushPass(&g_passes, R_PassType_Geo2D);
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
        u32 size = g_astroid_hexShape.count * sizeof(Vector2);
        g_astroid_hexBuffer = R_AllocStaticBuffer(g_astroid_hexBuffer, size, g_astroid_hexShape.data);
        node->params.mesh_vertices = g_astroid_hexBuffer;
        node->params.vert_count = g_astroid_hexShape.count;

        // Initialize group with N instances
        node->batches = R_MakeBatchList(sizeof(R_Hull2DInst));
        for(u32 i = 0; i < g_astroid_hexInstCount; i += 1)
        {
          R_Hull2DInst *hull_inst = (R_Hull2DInst*)R_PushBatchInst(GAME.arena, &node->batches, g_astroid_hexInstCount);
          Xform xform = g_astroid_hexXforms[i];
          Vector2 tr = xform.tr;
          f32     rt = xform.rt;
          Vector2 sc = xform.sc;

          Matrix model = MatrixIdentity();
          model = MatrixMultiply(model, MatrixScale(sc.x, sc.y, 1.0f));
          model = MatrixMultiply(model, MatrixTranslate(tr.x, tr.y, 0.0f));
          hull_inst->model = MatrixTranspose(model);
        }
      }

      // Initialize a 2D batch group for player
      {
        // Allocate a batch group
        R_BatchGroup2DNode *node = PushArray(GAME.arena, R_BatchGroup2DNode, 1);
        QueuePush(batch_groups->first, batch_groups->last, node);
        batch_groups->node_count += 1;

        // Allocate static mesh vertices buffer.
        u32 size = g_playerShape.count * sizeof(Vector2);
        g_playerBuffer = R_AllocStaticBuffer(g_playerBuffer, size, g_playerShape.data);
        node->params.mesh_vertices = g_playerBuffer;
        node->params.vert_count = g_playerShape.count;

        // Initialize group with N instances
        node->batches = R_MakeBatchList(sizeof(R_Hull2DInst));
        g_playerInst = (R_Hull2DInst*)R_PushBatchInst(GAME.arena, &node->batches, 1);
        Xform xform = *g_playerXform;
        Vector2 tr = xform.tr;
        f32     rt = xform.rt;
        Vector2 sc = xform.sc;

        Matrix model = MatrixIdentity();
        model = MatrixMultiply(model, MatrixScale(sc.x, sc.y, 1.0f));
        model = MatrixMultiply(model, MatrixTranslate(tr.x, tr.y, 0.0f));
        g_playerInst->model = MatrixTranspose(model);
      }
    }
    ProfEnd();
  }
}

