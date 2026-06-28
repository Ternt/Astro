// 2026-06-28

/////////////////////////////
//- Collider Shape Constructors

static P2_AABB P2_AABBFromMesh2D(Mesh2D mesh2d)
{
  // TODO: haven't tested for correctness
  P2_AABB result = zero_struct;
  f32 min_x = 0.0f, min_y = 0.0f;
  f32 max_x = 0.0f, max_y = 0.0f;
  for(u32 i = 0; i < mesh2d.vert_count; i += 1)
  {
    Vector2 vertex = mesh2d.data[i];
    min_x = Min(min_x, vertex.x);
    min_y = Min(min_y, vertex.y);
    max_x = Max(max_x, vertex.x);
    max_y = Max(max_y, vertex.y);
  }
  return result;
}

static P2_ColliderCircle P2_ColliderCircleFromMesh2D(Mesh2D mesh2d)
{
  P2_ColliderCircle result = zero_struct;  
  // TODO:
  return result;
}

static P2_Mesh2D P2_MeshFromMesh2D(Mesh2D mesh2d)
{
  P2_Mesh2D result = zero_struct;
  // TODO:
  return result;
}

/////////////////////////////
//- Support Functions for GJK

static Vector2 P2_FindFurthestPointInLocal(P2_Mesh2D mesh2d, Vector2 local_dir)
{
  f32 max_dot = 0.0;
  Vector2 result = Vector2Zero();
  for(u32 i = 0; i < mesh2d.vert_count; i += 1)
  {
    Vector2 v = mesh2d.vertices[i];
    f32 dot = Vector2DotProduct(v, local_dir);
    if(dot > max_dot)
    {
      max_dot = dot;
      result = v;
    }
  }
  return result;
}

/////////////////////////////
//- Collision Query Functions

static b32 P2_AABBVsAABB_(P2_BodyId a, P2_BodyId b)
{
  b32 result = false;
  // TODO:
  return result;
}

static b32 P2_AABBVsCircle_(P2_BodyId a, P2_BodyId b)
{
  b32 result = false;
  // TODO:
  return result;
}

static b32 P2_AABBVsMesh_(P2_BodyId a, P2_BodyId b)
{
  b32 result = false;
  // TODO:
  return result;
}

static b32 P2_CircleVsCircle_(P2_BodyId a, P2_BodyId b)
{
  b32 result = false;
  // TODO:
  return result;
}

static b32 P2_CircleVsMesh_(P2_BodyId a, P2_BodyId b)
{
  b32 result = false;
  // TODO:
  return result;
}

static b32 P2_MeshVsMesh_(P2_BodyId a, P2_BodyId b)
{
  P2_BodyData *a_body = P2_GetBodyData(a);
  P2_BodyData *b_body = P2_GetBodyData(b);

  Vector2 local_dir = Vector2One(); // direction in local space
  P2_Mesh2D a_collider = a_body->collider.mesh2d;
  P2_Mesh2D b_collider = b_body->collider.mesh2d;
  Vector2 a_support_point = P2_FindFurthestPointInLocal(a_collider, local_dir); // transform back to world space
  Vector2 b_support_point = P2_FindFurthestPointInLocal(b_collider, local_dir); // transform back to world space

  b32 result = false;
  // TODO:
  return result;
}

