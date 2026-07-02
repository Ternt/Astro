// 2026-07-02

#ifndef CORE_ARENA_H
#define CORE_ARENA_H

/////////////////////////////
//- Arena Header Size

#define ARENA_HEADER_SIZE 128

/////////////////////////////
//- Arena Flags

typedef U64 ArenaFlags;
enum 
{
  ArenaFlags_Growable = (1<<0),
};

/////////////////////////////
//- Arena Parameters

typedef struct ArenaParams ArenaParams;
struct ArenaParams {
  U64 chunk_cap;
  void *optional_buffer;
  ArenaFlags flags;
};

/////////////////////////////
//- Arena Header

typedef struct Arena Arena;
struct Arena {
  ArenaFlags flags;
  U64 chunk_pos;
  U64 chunk_cap;
  U64 pos;
};

/////////////////////////////
//- Arena Default Parameter Values

const U64 arena_default_chunk_cap = KB(64);
const ArenaFlags arena_default_flags = ArenaFlags_Growable;

/////////////////////////////
//- Arena Functions

#define Arena_Alloc(...) Arena_Alloc_(&(ArenaParams){ .chunk_cap=arena_default_chunk_cap, .optional_buffer=null, .flags=arena_default_flags, __VA_ARGS__ })
static Arena* Arena_Alloc_(ArenaParams *params);
static void   Arena_Release(Arena *arena);

static void   Arena_Push(Arena *arena, U64 size, U64 align);
static void   Arena_PopTo(Arena *arena, U64 pos);
static void   Arena_Pop(Arena *arena, U64 amt);

#endif // CORE_ARENA_H
