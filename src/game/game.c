// 2026-06-14

////////////////////////////
//- Game Includes

#include "core/core_helpers.c"
#include "core/core_profiling.c"
#include "core/core_arena.c"
#include "core/core_pool.c"
#include "core/core_strings.c"

////////////////////////////
//- Application Structure Functions

static force_inline void Game_Tick(void)
{
  ProfScopeBegin();
  ProfBegin();
  BeginDrawing();

  ClearBackground(BLACK);

  EndDrawing();
  ProfEnd(); 
  ProfScopeEnd();
}

static force_inline void Game_MainLoop(void)
{
#if PLATFORM_WEB
  emscripten_set_main_loop(Game_Tick, 0, 1);
#else
  while(!WindowShouldClose()){Game_Tick();}
#endif
}

int main(int argc, char *argv[])
{
  U32 w = atoi(argv[1]);
  U32 h = atoi(argv[2]);
  InitWindow(w,h,"Astro");

#if defined(GAME_TARGET_FPS)
  SetTargetFPS(GAME_TARGET_FPS);
#endif

  Game_EntryPoint(argc, argv);
  Game_MainLoop();
}
