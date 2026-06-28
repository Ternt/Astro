// 2026-06-19

static void DR_DrawLine2D_(Vector2 start_pos, Vector2 dir, f32 length, Color color)
{
  Vector2 line = Vector2Scale(dir, length);
  Vector2 end_pos = Vector2Add(start_pos, line);
  DrawLineV(start_pos, end_pos, color);
}
