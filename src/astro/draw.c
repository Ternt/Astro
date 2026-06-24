// 2026-06-19

static void
DR_DrawNGonConvex(Vector2 pos, f32 scale, u32 sides, b32 random)
{
}

static void
DR_DrawNGonConcave(Vector2 pos, f32 scale, u32 sides, b32 random)
{
}

static void DR_DrawBegin(Arena *arena)
{
}

static void DR_DrawEnd(void)
{
}

static void DR_DrawLine2D_(Vector2 start_pos, Vector2 dir, f32 length, Color color)
{
  Vector2 line = Vector2Scale(dir, length);
  Vector2 end_pos = Vector2Add(start_pos, line);
  DrawLineV(start_pos, end_pos, color);
}
