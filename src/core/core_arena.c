// 2026-06-14

static Arena *ArenaAlloc(const char *name, u32 chunk_cap, b32 growable)
{
  u32 chunk_cap_aligned = AlignPow2(chunk_cap, ARENA_DEFAULT_CAP);
  Arena *arena = (Arena*)malloc(chunk_cap_aligned);
  ProfMemoryAlloc(arena, chunk_cap_aligned, name);
  arena->curr = arena;
  arena->prev = null;
  arena->free = null;
  arena->name = (char*)name;
  arena->growable = growable;
  arena->chunk_cap = chunk_cap_aligned;
  arena->chunk_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
#if BUILD_DEBUG
  DebugCtx_InsertArena(arena);
#endif
  return arena;
}

static void ArenaRelease(Arena *arena)
{
  Arena *curr = arena->curr;
  for(Arena *prev = null; curr != null; curr = prev)
  {
    prev = curr->prev;
    ProfMemoryFree(curr, curr->name);
    free(curr);
  }
}

static void *ArenaPush(Arena *arena, u32 size, u32 align, b32 zero)
{
  Arena *curr = arena->curr;
  u32 pos_aln = AlignPow2(curr->pos, align);
  u32 pos_new = pos_aln + size;

  u32 size_to_zero = 0;
  if(zero)
  {
    size_to_zero = size;
  }

  // allocate new arena chunk.
  if(pos_new > curr->chunk_cap && arena->growable)
  { 
    Arena *old_chunk = curr;
    Arena *new_chunk = null;

    // search for free arena chunks in freelist.
    {
      Arena *prev_chunk;
      for(new_chunk = arena->free, prev_chunk = null; new_chunk != null; prev_chunk = new_chunk, new_chunk = new_chunk->prev)
      {
        if(new_chunk->chunk_cap >= AlignPow2(new_chunk->pos, align) + size)
        {
          // handle freelist connections.
          if(prev_chunk)
          {
            prev_chunk->prev = new_chunk->prev;
          }
          else
          {
            arena->free = new_chunk->prev;
          }
          break;
        }
      }
    }

    // if no chunks were found, allocate.
    if(new_chunk == null)
    {
      u32 chunk_cap = old_chunk->chunk_cap;
      if(size + ARENA_HEADER_SIZE > chunk_cap)
      {
        chunk_cap = AlignPow2(size + ARENA_HEADER_SIZE, align);
      }
      new_chunk = ArenaAlloc(old_chunk->name, chunk_cap, true);
    }
    new_chunk->chunk_pos = old_chunk->chunk_pos + old_chunk->chunk_cap;
    StackPush_N(arena->curr, new_chunk, prev);

    curr = new_chunk;
    pos_aln = AlignPow2(curr->pos, align);
    pos_new = pos_aln + size;
  }

  void *result = null;
  if(pos_new < curr->chunk_cap)
  {
    result = OffsetPtr(curr, pos_aln);
    curr->pos = pos_new;
    MemoryZero(result, size_to_zero);
  }
  return result;
}

static u32 ArenaPos(Arena *arena)
{
  Arena *curr = arena->curr;
  u32 pos = curr->chunk_pos + curr->pos;
  return pos;
}

static u32 ArenaCap(Arena *arena)
{
  Arena *curr = arena->curr;
  u32 pos = curr->chunk_pos + curr->chunk_cap;
  return pos;
}

static void ArenaPopTo(Arena *arena, u32 pos)
{
  u32 total_pos = ClampBot(ARENA_HEADER_SIZE, pos);
  Arena *curr = arena->curr;
  for(Arena *prev = null; curr->chunk_pos >= total_pos; curr = prev)
  {
    prev = curr->prev;
    curr->pos = ARENA_HEADER_SIZE;
    StackPush_N(arena->free, curr, prev);
  }
  arena->curr = curr;
  u32 new_pos = total_pos - curr->chunk_pos;
  arena->pos = new_pos;
}

static void ArenaPop(Arena *arena, u32 amt)
{
  u32 old_pos = ArenaPos(arena);
  u32 new_pos = old_pos;
  if(amt < old_pos)
  {
    new_pos = old_pos - amt;
  }
  ArenaPopTo(arena, new_pos);
}

static void ArenaClear(Arena* arena)
{
  ArenaPopTo(arena, 0);
}

static Temp TempBegin(Arena *arena)
{
  Temp temp = {arena, ArenaPos(arena)};
  return temp;
}

static void TempEnd(Temp temp)
{
  Arena *arena = temp.arena;
  ArenaPopTo(arena, temp.pos);
}

#define DBGCTX_ARENA_MAP_SLOT_COUNT 32

static void DebugCtx_Alloc(void)
{
  u32 chunk_cap = KB(8);
  debug_arena = (Arena*)malloc(chunk_cap);
  ProfMemoryAlloc(debug_arena, chunk_cap, "debug_arenas");
  debug_arena->curr = debug_arena;
  debug_arena->prev = null;
  debug_arena->free = null;
  debug_arena->name = "debug_arenas";
  debug_arena->growable = false;
  debug_arena->pos = ARENA_HEADER_SIZE;
  debug_arena->chunk_cap = chunk_cap;
  debug_arena->chunk_pos = 0;
  DEBUG = PushArray(debug_arena, DebugCtx, 1);
  DEBUG->interval = 0.05f;
  DEBUG->arena_map.slot_count = DBGCTX_ARENA_MAP_SLOT_COUNT;
  DEBUG->arena_map.slots = PushArray(debug_arena, DebugArenaMapNode*, DBGCTX_ARENA_MAP_SLOT_COUNT);
}

static void DebugCtx_Release(void)
{
  ProfMemoryFree(debug_arena, debug_arena->name);
  free(debug_arena);
}

const xxhash_seed = 0;

static DebugArenaMapNode *DebugCtx_GetNodeFromSlot(DebugArenaMapNode **slot, u64 hash)
{
  DebugArenaMapNode *result = null;
  for(result = *slot; result != null; result = result->next)
  {
    if(result->hash == hash)
    {
      break;
    }
  }
  return result;
}

static DebugArenaMapNode *DebugCtx_InsertArena(Arena *arena)
{
  u64 hash = XXH3_64bits_withSeed(arena->name, CstringLength(arena->name), xxhash_seed);
  u64 slot_idx = hash % DEBUG->arena_map.slot_count;
  DebugArenaMapNode **slot = &DEBUG->arena_map.slots[slot_idx];
  DebugArenaMapNode *existing_n = DebugCtx_GetNodeFromSlot(slot, hash);
  DebugArenaMapNode *result = null;
  if(existing_n != null)
  {
    result = existing_n;
  }
  else
  {
    result = PushArray(debug_arena, DebugArenaMapNode, 1);
    result->hash = hash;
    result->arena = arena;
    StackPush_N(*slot, result, hash_next);
    DebugArenaMapNodeList *iter = &DEBUG->arena_map.iter;
    QueuePush(iter->first, iter->last, result);
    iter->node_count += 1;
  }
  return result;
}

static void DebugCtx_TakeSnapshot(SnapshotRing *snapshots, Arena *arena)
{
  Snapshot snap = zero_struct;
  Arena *curr = arena->curr;
  snap.total_pos = ArenaPos(arena);
  snap.total_cap = ArenaCap(arena);
  snapshots->v[snapshots->count] = snap;
  snapshots->count = (snapshots->count + 1)%DEBUG_SNAPSHOT_CAP;
}
