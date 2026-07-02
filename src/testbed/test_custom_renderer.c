// 2026-07-1

////////////////////////////
//- Build Macro Configs

// profiling macros
#if !defined(PLATFORM_WEB) && defined(BUILD_DEBUG)
#define PROFILE_TRACY 1
#endif

////////////////////////////
//- Game Macro Configs

#define GAME_ARENA_SIZE MB(2)
#define R_DEFAULT_OGL_BUFFER_SIZE MB(64)

////////////////////////////
//- Game Includes

//- [h]
#include "game/game.h"

//- [c]
#include "game/game.c"

////////////////////////////
//- Game Loop Pipeline

static inline void 
Game_UpdatePhysics(void)
{
  ProfBegin();
  ProfEnd();
}

static inline void 
Game_UpdateState(void)
{
}

static inline void 
Game_DrawWorld(void)
{
}

static void
Game_DrawUI(void)
{
}

static void
Game_DrawDebug(void)
{
}

////////////////////////////
//- Game Entry Point (Game-specific Initializations)

static void Game_EntryPoint(int argc, char *argv[])
{
  Pool *object_pool = Pool_Alloc(GameObject);

  GameObject *object = Pool_GetBlock(object_pool);
}
