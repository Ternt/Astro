// 2026-06-28

#ifndef PHYSICS_BROADPHASE_H
#define PHYSICS_BROADPHASE_H

typedef struct P2_CollisionPair {
  P2_BodyId bodies[2];
  Vector2 correction[2];
} P2_CollisionPair;

#endif // PHYSICS_BROADPHASE_H
