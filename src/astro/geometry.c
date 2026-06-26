// 2026-06-17

static Mesh2D Geo_GenMesh2DConvex(u32 sides)
{
  Vector2 *data = PushArray(GEO->vertex_arena, Vector2, sides);
  f32 angle_inc = 360.0f / (f32)sides;
  for(u32 i = 0; i < sides; i += 1)
  {
    f32 angle = angle_inc * i;
    f32 rad = angle * DEG2RAD;
    f32 x = sinf(rad);
    f32 y = cosf(rad);
    data[i] = (Vector2){x,y};
  }

  Mesh2D result = zero_struct;
  result.data = data;
  result.vert_count = sides;
  GEO->total_vert_count += result.vert_count;
  return result;
}

static Mesh2D Geo_GenMesh2DConcave(u32 sides, f32 inner_radius_pct)
{
  // Gen points on unit circle.
  Vector2 temp[GEO_MAX_NGON_SIDES] = zero_struct;
  f32 angle_inc = 360.0f / (f32)sides;
  for(u32 i = 0; i < sides; i += 1)
  {
    f32 angle = angle_inc * i;
    f32 rad = angle * DEG2RAD;
    f32 x = sinf(rad);
    f32 y = cosf(rad);
    temp[i] = (Vector2){x,y};
  }

  // Deform some points towards inner radius 
  // percentage to make the N-gon concave.
  Vector2 *data = PushArray(GEO->vertex_arena, Vector2, sides * 3);
  for(u32 i = 0; i < sides - 1; i += 1)
  {
  }

  Mesh2D result = zero_struct;
  result.data = data;
  result.vert_count = 3*sides;
  GEO->total_vert_count += result.vert_count;
  return result;
}

static void Geo_Init(void)
{
  Arena *vertex_arena = ArenaAlloc("vertices", KB(1));
  GEO = PushArray(RENDER->arena, Geo_Ctx, 1);
  GEO->vertex_arena = vertex_arena;
}
