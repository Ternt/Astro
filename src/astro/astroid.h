// 2026-06-23

#ifndef ASTROID_H
#define ASTROID_H

////////////////////////////
//- Astroid GameObject Types & Data Structures

typedef struct Astroid {
  // transforms
  Vector2 tr;
  f32     rt;
  Vector2 sc;

  // geometry
  Vector2 *vertices;
  u32 vert_count;
} Astroid;

typedef struct AstroidArray {
  Astroid *v;
  u32 count;
  u32 cap;
} AstroidArray;

////////////////////////////
//- Astroid GameObject Functions

static AstroidArray MakeAstroidArray(Arena *arena, u32 cap);

#endif // ASTROID_H
