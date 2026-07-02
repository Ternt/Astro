// 2026-07-02

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

/////////////////////////////
//- Pool Header Size

#define POOL_HEADER_SIZE 128

/////////////////////////////
//- Pool Flags

typedef U32 PoolFlags;
enum 
{
  PoolFlags_Growable = (1<<0),
};

/////////////////////////////
//- Pool Parameters

typedef struct PoolParams PoolParams;
struct PoolParams {
  U64 chunk_size;
  U64 block_size;
  void *optional_buffer;
  PoolFlags flags;
  const char *name;
  const char *allocation_file;
  U32 allocation_line;
};

/////////////////////////////
//- Pool Block

typedef struct PoolBlock PoolBlock;
struct PoolBlock {
  PoolBlock *next;
  PoolBlock *prev;
  void *data;
};

/////////////////////////////
//- Pool Header

typedef struct Pool Pool;
struct Pool {
  // chunk stack
  Pool *current;
  Pool *prev;
  U32 chunk_count;
  // general info
  const char *name;
  PoolFlags flags;
  U64 chunk_size;
  U64 block_size;
  U64 free_block_count;
  // block free list
  PoolBlock *first_block;
  PoolBlock *last_block;
  PoolBlock *free_blocks;
  // allocation data
  const char *allocation_file;
  U32 allocation_line;
};

/////////////////////////////
//- Arena Default Parameter Values

const U64 pool_default_chunk_size = KB(64);
const ArenaFlags pool_default_flags = ArenaFlags_Growable;

/////////////////////////////
//- Pool Functions

#define Pool_Alloc(T, ...) Pool_Alloc_(&(PoolParams){ .chunk_size=pool_default_chunk_size, .block_size = sizeof(T), .optional_buffer=null, .flags=pool_default_flags, .name="", .allocation_file= __FILE__, .allocation_line=__LINE__, __VA_ARGS__ })
static Pool* Pool_Alloc_(PoolParams *params);
static void  Pool_Release(Pool *pool);

static void* Pool_GetBlock(Pool *pool);
static void  Pool_FreeBlock(Pool *pool, void *block);

#endif // POOL_ALLOCATOR_H
