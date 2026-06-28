// 2026-06-14

////////////////////////////
//- Game World and Utility Helpers

static void Game_HandleDebugControls(void)
{
  if(IsKeyDown(KEY_D))
  {
    GAME.is_debug_arenas = false;
  }
  if(IsKeyDown(KEY_X))
  {
    GAME.is_debug_arenas = true;
  }
}

static void Game_HandleCameraControls(void)
{
  // Handle click dragging
  if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
  {
    Vector2 delta = GetMouseDelta();
    GAME.camera_2d.target = Vector2Subtract(GAME.camera_2d.target, Vector2Scale(delta, 1.0f/GAME.camera_zoom));
  }

  // Handle zooming
  Vector2 wheel = GetMouseWheelMoveV();
  GAME.camera_zoom += (GAME.camera_zoom * 0.1f * wheel.y);
  GAME.camera_zoom = Max(GAME.camera_zoom, 0.01f);
  GAME.camera_2d.zoom = GAME.camera_zoom;
}

static void Game_DrawGrid(float w, float h, float cell_size, Color color)
{
  float half_w = w * 0.5f;
  float half_h = h * 0.5f;
  int cols = (u32)(w / cell_size);
  int rows = (u32)(h / cell_size);
  for(u32 i = 0; i <= cols; i += 1)
  {
    float x = -half_w + i * cell_size;
    DrawLineV((Vector2){x, -half_h}, (Vector2){x, half_h}, color);
  }
  for(u32 i = 0; i <= rows; i += 1)
  {
    float y = -half_h + i * cell_size;
    DrawLineV((Vector2){-half_w, y}, (Vector2){half_w, y}, color);
  }
}

static void Game_DrawOrigin(Color color)
{
  DrawLineV((Vector2){-1, 0}, (Vector2){1,0}, color);
  DrawLineV((Vector2){ 0,-1}, (Vector2){0,1}, color);
}

static void Game_DebugArena(void)
{
#if BUILD_DEBUG
  if(GAME.is_debug_arenas)
  {
    DebugArenaMap *arena_map = &DEBUG->arena_map;
    u32 arena_count = arena_map->iter.node_count;
    b32 snapshot_taken = false;

    f32 w = GetScreenWidth()*0.975;
    f32 h = GetScreenHeight();

    f32 padding = 5.0f;
    f32 max_text_width = 140.0f;
    f32 snapshot_bar_p = 2;  // padding
    f32 snapshot_bar_h = 80; // height

    u32 panel_off = 0;
    f32 panel_1_w = max_text_width;
    f32 panel_1_h = (snapshot_bar_p * (arena_count + 1)) + (snapshot_bar_h * arena_count);
    f32 panel_1_x = padding;
    f32 panel_1_y = ClampBot(h - panel_1_h - padding, 0);

    f32 panel_2_w = w-max_text_width;
    f32 panel_2_h = (snapshot_bar_p * (arena_count + 1)) + (snapshot_bar_h * arena_count);
    f32 panel_2_x = max_text_width + (padding*2.0);
    f32 panel_2_y = ClampBot(h - panel_2_h, 0);

    f32 snapshot_bar_w = ((w-max_text_width-(padding*2.0)-(snapshot_bar_p*DEBUG_SNAPSHOT_CAP))/DEBUG_SNAPSHOT_CAP); // width

    DebugArenaMapNodeList *arenas = &arena_map->iter;
    for(DebugArenaMapNode *n = arenas->first; n != null; n = n->next, panel_off += 1)
    {
      Arena *arena = n->arena;
      SnapshotRing *snapshots = &n->snapshots;
      if(GAME.elapsed >= DEBUG->interval)
      {
        DebugCtx_TakeSnapshot(snapshots, arena);
        snapshot_taken |= true;
      }

      f32 ar_bytes_font_size = 16;
      f32 ar_dbug_text_h = DR_FONT_SIZE + ar_bytes_font_size;
      f32 ar_name_x = panel_1_x;
      f32 ar_name_y = (panel_1_y + (snapshot_bar_p + snapshot_bar_h) * panel_off);
      f32 ar_bytes_x = panel_1_x;
      f32 ar_bytes_y = (panel_1_y + (snapshot_bar_p + snapshot_bar_h) * panel_off) + DR_FONT_SIZE;

      for EachIdx(i, 0, DEBUG_SNAPSHOT_CAP, 1)
      {
        Snapshot snap = snapshots->v[i];
        if(snap.total_cap == 0) break;
        f32 progress = (f32)snap.total_pos/(f32)snap.total_cap;

        Color bar_color = LIME;
        if(InRange(progress, 0.50, 0.75)) bar_color = ORANGE;
        if(InRange(progress, 0.75, 1.00)) bar_color = RED;

        u32 bar_off = i;
        f32 bar_w = snapshot_bar_w;
        f32 bar_h = ClampBot(progress*snapshot_bar_h, 1.0);
        f32 bar_x = (panel_2_x + (snapshot_bar_p + snapshot_bar_w) * bar_off);
        f32 bar_y = (panel_2_y + (snapshot_bar_p + snapshot_bar_h) * panel_off) + (snapshot_bar_h + snapshot_bar_p - bar_h);
        DR_DrawRectangle(bar_x, bar_y, bar_w, bar_h, bar_color);
      }

      DR_DrawText(TextFormat("arena:%s", arena->name), ar_name_x, ar_name_y, WHITE);
      DR_DrawTextEx(TextFormat("%u:%u bytes", ArenaPos(arena), ArenaCap(arena)), ar_bytes_x, ar_bytes_y, ar_bytes_font_size, GRAY);
    }
    if(snapshot_taken)
    {
      GAME.elapsed = 0.0f;
    }
  }
#endif
}

////////////////////////////
//- Application Structure Functions

static force_inline void Game_Tick(void)
{
  ProfScopeBegin();
  ProfBegin("Game_Tick");
  Temp temp = TempBegin(GAME.arena);
  Game_UpdatePhysics();
  Game_UpdateState();

  BeginDrawing();
  R_FrameBegin();
    ClearBackground(BLACK);
    Game_DrawWorld();
    Game_DrawUI();
    Game_DrawDebug();
  R_FrameEnd();
  EndDrawing();

  f32 end = GetTime();
  f32 elapsed = end - GAME.start;
  GAME.elapsed += elapsed;
  GAME.start = end;
  TempEnd(temp);
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
  u32 w = atoi(argv[1]);
  u32 h = atoi(argv[2]);
  InitWindow(w,h,"Astro");

  // load font
  const char *font_path = TextFormat("%s/%s", GetWorkingDirectory(), "assets/jetbrains-mono.ttf");
  GAME.default_font = LoadFontEx(font_path, 32, 0, 0);

  // camera values
  w = GetScreenWidth();
  h = GetScreenHeight();
  GAME.camera_zoom = Min(w,h)/256.0f;

  // initialize camera
  GAME.camera_2d.target   = (Vector2){0, 0};
  GAME.camera_2d.offset   = (Vector2){w/2.f, h/2.f};
  GAME.camera_2d.zoom     = GAME.camera_zoom;
  GAME.camera_2d.rotation = 0.0f;

  // debugging
  GAME.is_debug_arenas = true;
  DebugCtx_Alloc();

  GAME.arena = ArenaAlloc("game_core", GAME_ARENA_SIZE, true);
  GAME.vertex_arena = ArenaAlloc("vertices", KB(1), false);

  R_Init();
  P2_Init();

  // Main Loop
  Game_EntryPoint(argc, argv);
  GAME.start = GetTime();
  Game_MainLoop();

  P2_Quit();
  R_Quit();

  ArenaRelease(GAME.vertex_arena);
  ArenaRelease(GAME.arena);
  DebugCtx_Release();

  // unload font
  UnloadFont(GAME.default_font);
}
