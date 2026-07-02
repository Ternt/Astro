// 2026-06-14
//
// Abstracts the code difference between 
// browser and desktop platform.

#ifndef GAME_H
#define GAME_H

////////////////////////////
//- Third Party Includes

#if defined(PLATFORM_WEB)
# include <GLES3/gl3.h>
# include <emscripten/emscripten.h>
#else
# include "third_party/raylib/external/glad.h"
#endif
#include "third_party/raylib/raylib.h"
#include "third_party/raylib/raymath.h"
#include "third_party/raylib/rlgl.h"

////////////////////////////
//- Game Includes

#include "core/core_helpers.h"
#include "core/core_profiling.h"
#include "core/core_arena.h"
#include "core/core_pool.h"
#include "core/core_strings.h"
#include "game/game_objects.h"

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
#define GAME_ARENA_SIZE KB(64)
#endif

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
