// 2026-06-23

static AstroidArray MakeAstroidArray(Arena *arena, u32 cap)
{
  AstroidArray result = zero_struct;
  result.v = PushArray(arena, Astroid, cap);
  result.cap = cap;
  return result;
}
