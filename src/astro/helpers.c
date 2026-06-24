// 2026-06-14

static inline Vector2 WorldToScreen(Vector2 v)
{
  return (Vector2){ v.x, -v.y };
}

static inline f32 RandF32(f32 a, f32 b)
{
  return a + (f32)rand() / (f32)(RAND_MAX) * (b - a);
}

static inline u8 RandU8(u8 a, u8 b)
{
  return a + rand() % (b - a + 1);
}
