// 2026-06-23
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
#include "astroid.h"
#include "game.h"

#include "helpers.c"
#include "arena.c"
#include "render.c"
#include "geometry.c"
#include "draw.c"
#include "astroid.c"
#include "game.c"

////////////////////////////
//- Game Globals

b32 is_debug_arenas = true;

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
  if(IsKeyDown(KEY_MINUS))
  {
    is_debug_arenas = !is_debug_arenas;
  }

  ProfBegin("Game_UpdateState");
  ProfEnd();
}

static inline void 
Game_DrawWorld(void)
{
  ProfBegin("R_DrawAll");
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
  if(InRange(fps, 15, 30)) fpsColor = ORANGE;
  if(InRange(fps,  0, 14)) fpsColor = RED;
  DR_DrawText(TextFormat("%2i FPS", fps), 0, 0, fpsColor);

  if(is_debug_arenas)
  {
    u32 w = GetScreenWidth();
    u32 h = GetScreenHeight();
    f32 ar_dbug_bar_w = 0.4*w; // width
    f32 ar_dbug_bar_h = 50; // height
    f32 ar_dbug_bar_p = 2; // padding

    f32 panel_w = ar_dbug_bar_w;
    f32 panel_h = (ar_dbug_bar_p * (debug_ctx->count + 1)) + (ar_dbug_bar_h * debug_ctx->count);
    f32 panel_pos_x = 0;
    f32 panel_pos_y = ClampBot(h - panel_h, 0);
    u32 off = 0;
    for(ArenaNode *n = debug_ctx->first; n != null; n = n->next, off += 1)
    {
      Arena *arena = n->v;
      f32 progress = (f32)arena->pos/(f32)arena->cap;

      Color bar_color = LIME;
      if(InRange(progress, 0.50, 0.74)) bar_color = ORANGE;
      if(InRange(progress, 0.75, 1.00)) bar_color = RED;

      f32 bar_x = panel_pos_x;
      f32 bar_y = panel_pos_y + (ar_dbug_bar_p + ar_dbug_bar_h) * off;
      f32 bar_w = progress*ar_dbug_bar_w;
      f32 bar_h = ar_dbug_bar_h;
      f32 bar_bg_x = bar_x;
      f32 bar_bg_y = bar_y;
      f32 bar_bg_w = ar_dbug_bar_w;
      f32 bar_bg_h = bar_h;
      DR_DrawRectangle(bar_bg_x, bar_bg_y, bar_bg_w, bar_bg_h, ((Color){20, 20, 20, 255}));
      DR_DrawRectangle(bar_x, bar_y, bar_w, bar_h, bar_color);

      f32 outline_x = bar_x;
      f32 outline_y = bar_y;
      f32 outline_w = bar_w;
      f32 outline_h = ar_dbug_bar_h;
      DR_DrawRectangleLines(outline_x, outline_y, outline_w, outline_h, LIGHTGRAY);

      f32 ar_bytes_font_size = 16;
      f32 ar_dbug_text_h = DR_FONT_SIZE + ar_bytes_font_size;
      f32 ar_name_x  = bar_x + bar_w;
      f32 ar_name_y  = bar_y + ((bar_h*0.5) - (ar_dbug_text_h*0.5));
      f32 ar_bytes_x = bar_x + bar_w;
      f32 ar_bytes_y = bar_y + ((bar_h*0.5) - (ar_dbug_text_h*0.5) + DR_FONT_SIZE);
      DR_DrawText(TextFormat("%s", arena->name), ar_name_x, ar_name_y, WHITE);
      DR_DrawTextEx(TextFormat("%u bytes", arena->pos), ar_bytes_x, ar_bytes_y, ar_bytes_font_size, GRAY);
    }
  }

  ProfEnd();
}

////////////////////////////
//- Game Entry Point (Game-specific Initializations)

static void Game_EntryPoint(int argc, char *argv[])
{
  Arena *astroid_pool = ArenaAlloc("astroids", MB(2));
  AstroidArray astroids = MakeAstroidArray(astroid_pool, 1024);
}

