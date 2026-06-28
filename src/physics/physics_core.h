// 2026-06-28

#ifndef PHYSICS_CORE_H
#define PHYSICS_CORE_H

/////////////////////////////
//- Physics2D Entity Ids

typedef u64 P2_WorldId;
typedef u64 P2_BodyId;

#define null_id 0
#define P2_MAX_WORLDS 100
#define P2_MAX_BODIES max_u32

/////////////////////////////
//- Physics2D Collision Shape Typedefs

// AABB Collision Shape
typedef struct P2_AABB {
  Vector2 min;
  Vector2 max;
} P2_AABB;

// Circle Collision Shape
typedef struct P2_Circle {
  Vector2 pos;
  f32 radius;
} P2_Circle;

// N-Gon Collision Shape
typedef struct P2_Mesh2D {
  Vector2 pos;
  Vector2 *vertices;
  u32 vert_count;
} P2_Mesh2D;

// Collision Shape Type
typedef u32 P2_ColliderType;
enum
{
  P2_ColliderType_Null   = -1,
  P2_ColliderType_AABB   = 0,
  P2_ColliderType_Circle = 1,
  P2_ColliderType_Mesh   = 2,
  P2_ColliderType_COUNT,
};

// Collision Shape
typedef union P2_Collider { 
  P2_ColliderType type;
  struct {
    P2_AABB aabb;
    P2_Circle circle;
    P2_Mesh2D mesh2d;
  };
} P2_Collider;

/////////////////////////////
//- Physics2D Body Data

typedef struct P2_BodyParams {
  f32 mass;
  Vector2 pos;
  Vector2 vel;
  Vector2 acc;
  f32 rot;
  f32 angular_acc;
  P2_Collider collider;
} P2_BodyParams;

typedef struct P2_BodyData P2_BodyData;
struct P2_BodyData {
  P2_BodyData *next;
  P2_BodyData *prev;
  f32 mass;
  f32 inv_mass;
  Vector2 pos;
  Vector2 vel;
  Vector2 acc;
  f32 rot;
  f32 angular_vel;
  f32 angular_acc;
  P2_Collider collider;
};

/////////////////////////////
//- Physics2D World Typedefs

typedef struct P2_WorldParams {
  f32 gravity;
  Vector2 bounds;
} P2_WorldParams;

typedef struct P2_WorldData {
  f32 gravity;
  Vector2 bounds;
  P2_BodyData *first_body;
  P2_BodyData *last_body;
  P2_BodyData *free_bodies;
  u32 body_count;
} P2_WorldData;

/////////////////////////////
//- Collision Functions

// Collider shape constructors
static P2_AABB   P2_AABBFromMesh2D(Mesh2D mesh);
static P2_Circle P2_CircleFromMesh2D(Mesh2D mesh);
static P2_Mesh2D P2_MeshFromMesh2D(Mesh2D mesh);

// Support functions for GJK
static Vector2 P2_FindFurthestPointInLocal(P2_Mesh2D mesh2d, Vector2 local_dir);

// Collision queries
static b32 P2_AABBVsAABB_(P2_BodyId a, P2_BodyId b);
static b32 P2_AABBVsCircle_(P2_BodyId a, P2_BodyId b);
static b32 P2_AABBVsMesh_(P2_BodyId a, P2_BodyId b);
static b32 P2_CircleVsCircle_(P2_BodyId a, P2_BodyId b);
static b32 P2_CircleVsMesh_(P2_BodyId a, P2_BodyId b);
static b32 P2_MeshVsMesh_(P2_BodyId a, P2_BodyId b);

// Collision query combinations
#define P2_AABBVsAABB(a,x)      P2_AABBVsAABB_(a,x)
#define P2_AABBVsCircle(a,x)    P2_AABBVsCircle_(a,x)
#define P2_AABBVsMesh(a,x)      P2_AABBVsMesh_(a,x)
#define P2_CircleVsAABB(c,x)    P2_AABBVsCircle_(x,c)
#define P2_CircleVsCircle(c,x)  P2_CircleVsCircle_(c,x)
#define P2_CircleVsMesh(c,x)    P2_CircleVsMesh_(c,x)
#define P2_MeshVsAABB(m,x)      P2_AABBVsMesh_(x,m)
#define P2_MeshVsCircle(m,x)    P2_CircleVsMesh_(x,m)
#define P2_MeshVsMesh(m,x)      P2_MeshVsMesh_(m,x)

/////////////////////////////
//- Core Functions

// World functions
static P2_WorldId   P2_CreateWorld(P2_WorldParams *params);
static void         P2_DestroyWorld(P2_WorldId world_id);

// Body functions
static P2_BodyId    P2_CreateBody(P2_WorldId world_id, P2_BodyParams *params);
static void         P2_DestroyBody(P2_WorldId world_id, P2_BodyId body_id);
static P2_BodyData* P2_GetBodyData(P2_BodyId body_id);
static Vector2      P2_GetBodyPosition(P2_BodyId body_id);
static f32          P2_GetBodyRotation(P2_BodyId body_id);
static P2_Collider  P2_GetBodyCollider(P2_BodyId body_id);
static void         P2_ApplyForce(P2_WorldId world_id, P2_BodyId body_id, Vector2 force);

#endif // PHYSICS_CORE_H
