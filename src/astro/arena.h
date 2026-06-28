// 2026-06-14

#ifndef ARENA_H
#define ARENA_H

////////////////////////////
//- Arena Constants

#define ARENA_HEADER_SIZE (u32)Clamp(32, sizeof(Arena), 128)
#define ARENA_DEFAULT_CAP KB(4)
#define DEBUG_SNAPSHOT_CAP 64

////////////////////////////
//- Arena Type Definitions

// Arena Header Type
typedef struct Arena Arena;
struct Arena {
  Arena *curr;
  Arena *prev;
  Arena *free;
  char *name;
  b32 growable;
  u32 chunk_cap;
  u32 chunk_pos;
  u32 pos;
};

// Temp Type
typedef struct Temp {
  Arena *arena;
  u32 pos;
} Temp;

////////////////////////////
//- Memory Debugging Type Definitions

// Snapshot of Arena
typedef struct Snapshot {
  u32 total_pos;
  u32 total_cap;
} Snapshot;

// Snapshot Ring Buffer Type
typedef struct SnapshotRing {
  Snapshot v[DEBUG_SNAPSHOT_CAP];
  u32 count;
} SnapshotRing;

// Node Type Containing Memory Snapshots
typedef struct DebugArenaMapNode DebugArenaMapNode;
struct DebugArenaMapNode {
  DebugArenaMapNode *hash_next;
  DebugArenaMapNode *next;
  u64 hash;
  Arena *arena;
  SnapshotRing snapshots;
};

typedef struct DebugArenaMapNodeList {
  DebugArenaMapNode *first;
  DebugArenaMapNode *last;
  u32 node_count;
} DebugArenaMapNodeList;

// Debug Map for Arenas
typedef struct DebugArenaMap {
  DebugArenaMapNode **slots;
  DebugArenaMapNodeList iter;
  u32 slot_count;
} DebugArenaMap;

// Debugging Context
typedef struct DebugCtx {
  DebugArenaMap arena_map;
  f32 interval;
} DebugCtx;

////////////////////////////
//- Arena Globals

static Arena *debug_arena = null;
static DebugCtx *DEBUG = null;

////////////////////////////
//- Arena Functions

// core arena operations
static Arena *ArenaAlloc(const char *name, u32 chunk_cap, b32 growable);
static void   ArenaRelease(Arena *arena);
static void  *ArenaPush(Arena *arena, u32 size, u32 align, b32 zero);
static void   ArenaPopTo(Arena *arena, u32 pos);
static void   ArenaPop(Arena *arena, u32 amt);
static void   ArenaClear(Arena *arena);
static Temp   TempBegin(Arena *arena);
static void   TempEnd(Temp temp);

// function-like helper macros
#define PushArrayAlignedNoZero(a,T,c,align) ArenaPush(a,(c)*sizeof(T),align,false)
#define PushArrayNoZero(a,T,c) PushArrayAlignedNoZero(a,T,c,Max(8,AlignOf(T)))
#define PushArrayAligned(a,T,c,align) ArenaPush(a,(c)*sizeof(T),align,true)
#define PushArray(a,T,c) PushArrayAligned(a,T,c,Max(8,AlignOf(T)))
#define PushStructAligned(a,T,align) ArenaPush(a,sizeof(T),align,true)
#define PushStruct(a,T) PushStructAligned(a,T,Max(8,AlignOf(T)))

// arena debug operations
static void DebugCtx_Alloc(void);
static void DebugCtx_Release(void);
static DebugArenaMapNode *DebugCtx_InsertArena(Arena *arena);
static void DebugCtx_RemoveArena(Arena *arena);
static void DebugCtx_TakeSnapshot(SnapshotRing *snapshots, Arena *arena);

#endif // ARENA_H
