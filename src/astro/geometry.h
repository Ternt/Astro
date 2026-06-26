// 2026-06-17

#ifndef GEOMETRY_H
#define GEOMETRY_H

////////////////////////////
//- Geometry Constants

#define GEO_MAX_NGON_SIDES 12
#define GEO_MAX_NGON_COUNT 1024
#define GEO_DEFAULT_INNER_RADIUS_PCT 0.5

////////////////////////////
//- Geometry Type Definitions

typedef struct Mesh2D {
  Vector2 *data;
  u32 vert_count;
} Mesh2D;

typedef struct Geo_Ctx {
  Arena *vertex_arena;
  u32 total_vert_count;
} Geo_Ctx;

////////////////////////////
//- Geometry Globals

static Geo_Ctx *GEO = zero_struct;

////////////////////////////
//- Geometry Functions

static Mesh2D Geo_GenMesh2DConvex(u32 sides);
static Mesh2D Geo_GenMesh2DConcave(u32 sides, f32 inner_radius_pct);

static void Geo_Init(void);
static void Geo_Quit(void);

#endif // GEOMETRY_H
