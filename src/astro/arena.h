// 2026-06-14

#ifndef ARENA_H
#define ARENA_H

////////////////////////////
//- Arena Constants

#define ARENA_HEADER_SIZE Clamp(32, sizeof(Arena), 64)
#define ARENA_DEFAULT_CAP KB(1)
#define DEBUG_SNAPSHOT_CAP 64

////////////////////////////
//- Arena Type Definitions

// Arena Header Type
typedef struct Arena {
  const char *name;
  u32 cap;
  u32 pos;
} Arena;

// Temp Type
typedef struct Temp {
  Arena *arena;
  u32 pos;
} Temp;

////////////////////////////
//- Memory Debugging Type Definitions

// Snapshot of Arena
typedef struct Snapshot {
  u32 pos;
  u32 cap;
} Snapshot;

// Snapshot Ring Buffer Type
typedef struct SnapshotRing {
  Snapshot v[DEBUG_SNAPSHOT_CAP];
  u32 count;
} SnapshotRing;

// Node Type Containing Memory Snapshots
typedef struct ArenaNode ArenaNode;
struct ArenaNode {
  ArenaNode *next;
  Arena *arena;
  SnapshotRing snapshots;
};

// Debugging Context
typedef struct DebugCtx {
  ArenaNode *first;
  ArenaNode *last;
  u32 count;
  f32 interval;
} DebugCtx;

////////////////////////////
//- Arena Globals

static Arena *debug_arena = null;
static DebugCtx *debug_ctx = null;

////////////////////////////
//- Arena Functions

// core arena operations
static Arena *ArenaAlloc(const char *name, u32 cap);
static void   ArenaRelease(Arena *arena);
static void  *ArenaPush(Arena *arena, u32 size, u32 align);
static void   ArenaPopTo(Arena *arena, u32 pos);
static void   ArenaPop(Arena *arena, u32 amt);
static void   ArenaClear(Arena *arena);
static Temp   TempBegin(Arena *arena);
static void   TempEnd(Temp temp);

// arena debug operations
static void DebugCtx_Alloc(void);
static void DebugCtx_Release(void);
static void DebugCtx_PushArena(Arena *arena);
static void DebugCtx_TakeSnapshot(SnapshotRing *snapshots, Arena *arena);

// function-like helper macros
#define PushArrayAligned(a,T,c,align) ArenaPush(a,(c)*sizeof(T),align)
#define PushArray(a,T,c) PushArrayAligned(a,T,c,Max(8,AlignOf(T)))
#define PushStructAligned(a,T,align) ArenaPush(a,sizeof(T),align)
#define PushStruct(a,T) PushStructAligned(a,T,Max(8,AlignOf(T)))

#endif // ARENA_H
