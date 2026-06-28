// 2026-06-14
//
// Abstracts the code difference between 
// browser and desktop platform.

#ifndef GAME_H
#define GAME_H

////////////////////////////
//- Third Party Includes

#if !defined(PLATFORM_WEB)
#include "third_party/raylib/external/glad.h"
#else
#include <GLES3/gl3.h>
#endif
#include "third_party/raylib/raylib.h"
#include "third_party/raylib/raymath.h"
#include "third_party/raylib/rlgl.h"

////////////////////////////
//- Game Includes

#include "core/core_helpers.h"
#include "core/core_profiling.h"
#include "core/core_arena.h"
#include "core/core_render.h"
#include "core/core_geometry.h"
#include "core/core_draw.h"
#include "physics/physics.h"

////////////////////////////
//- Build Configuration

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
//- Layer Configuration

#ifndef GAME_ARENA_SIZE
#define GAME_ARENA_SIZE KB(4)
#endif

////////////////////////////
//- Emscripten Includes

#if PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

////////////////////////////
//- Game Typedefs

typedef struct Game {
  Arena *arena;
  Arena *vertex_arena;
  Font default_font;
  Camera2D camera_2d;
  f32 camera_zoom;
  f32 start;
  f32 elapsed;
  u32 total_vert_count;
#if BUILD_DEBUG
  b32 is_debug_arenas;
#endif
} Game;

////////////////////////////
//- Game Globals

static Game GAME = zero_struct;

////////////////////////////
//- Game World and Utility Helpers

// helpers
static void Game_HandleDebugControls(void);
static void Game_HandleCameraControls(void);
static void Game_DrawGrid(f32 w, f32 h, f32 cell_size, Color color);
static void Game_DrawOrigin(Color color);
static void Game_DebugArena(void);

////////////////////////////
//- Application Structure Functions

// defined by the user
static inline void Game_UpdatePhysics(void);
static inline void Game_UpdateState(void);
static inline void Game_DrawWorld(void);
static inline void Game_DrawUI(void);
static inline void Game_DrawDebug(void);
static void Game_EntryPoint(int argc, char *argv[]);

// main loop, internal, not-user defined
static force_inline void Game_Tick(void);
static force_inline void Game_MainLoop(void);

#endif // GAME_H
