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

static P2_Circle P2_CircleFromMesh2D(Mesh2D mesh2d)
{
  P2_Circle result = zero_struct;  
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

/////////////////////////////
//- World Functions

static P2_WorldId P2_CreateWorld(P2_WorldParams *params)
{
  P2_WorldId result = null_id;
  if(P2->world_count < P2_MAX_WORLDS)
  {
    P2_WorldData *world = &P2->worlds[P2->world_count];
    world->gravity = params->gravity;
    world->bounds = params->bounds;
    world->first_body = null;
    world->last_body = null;
    world->free_bodies = null;
    world->body_count = 0;
    result = (P2_WorldId)world;
    P2->world_count += 1;
  }
  return result;
}

static inline P2_WorldData *P2_GetWorldData(P2_WorldId world_id)
{
  P2_WorldData *result = (P2_WorldData*)world_id;
  return result;
}

static void P2_DestroyWorld(P2_WorldId world_id)
{
  if(world_id == null_id) return;
  P2_WorldData *world = &P2->worlds[world_id];
  MemoryZeroStruct(world);
  P2->world_count -= 1;
}

/////////////////////////////
//- Body Functions

static P2_BodyId P2_CreateBody(P2_WorldId world_id, P2_BodyParams *params)
{
  P2_BodyId result = null_id;
  if(world_id != null_id)
  {
    P2_WorldData *world = P2_GetWorldData(world_id);
    P2_BodyData *body_data = PushArrayNoZero(P2->bodies_arena, P2_BodyData, 1);
    DLLPushBack(world->first_body, world->last_body, body_data);

    body_data->mass = params->mass;
    body_data->inv_mass = 1.0f/body_data->mass;
    body_data->pos = params->pos;
    body_data->vel = params->vel;
    body_data->acc = params->acc;
    body_data->collider = params->collider;

    body_data->rot = params->rot;
    body_data->angular_vel = 0;
    body_data->angular_acc = params->angular_acc;

    world->body_count += 1;
    P2->total_body_count += 1;
    result = (P2_BodyId)body_data;
  }
  return result;
}

static void P2_DestroyBody(P2_WorldId world_id, P2_BodyId body_id)
{
  if(body_id != null_id)
  {
    P2_WorldData *world = P2_GetWorldData(world_id);
    P2_BodyData *body = P2_GetBodyData(body_id);
    DLLRemove(world->first_body, world->last_body, body);
    world->body_count -= 1;
    P2->total_body_count -= 1;
  }
}

static inline P2_BodyData *P2_GetBodyData(P2_BodyId body_id)
{
  P2_BodyData *result = (P2_BodyData*)body_id;
  return result;
}

static inline Vector2 P2_GetBodyPosition(P2_BodyId body_id)
{
  P2_BodyData *body_data = P2_GetBodyData(body_id);
  return body_data->pos;
}

static inline f32 P2_GetBodyRotation(P2_BodyId body_id)
{
  P2_BodyData *body_data = P2_GetBodyData(body_id);
  return body_data->rot;
}

static P2_Collider P2_GetBodyCollider(P2_BodyId body_id)
{
  P2_BodyData *body_data = P2_GetBodyData(body_id);
  return body_data->collider;
}

static void P2_ApplyForce(P2_WorldId world_id, P2_BodyId body_id, Vector2 force)
{
  if(body_id != null_id && world_id != null_id)
  {
    P2_WorldData *world = P2_GetWorldData(world_id);
    P2_BodyData *body = P2_GetBodyData(body_id);
    body->acc = Vector2Scale(force, body->inv_mass);
  }
}

