// 2026-06-14

static Arena *ArenaAlloc(const char *name, u32 cap)
{
  Arena *arena = (Arena*)malloc(cap);
  arena->pos = ARENA_HEADER_SIZE;
  arena->cap = cap;
  arena->name = name;
  DebugCtx_PushArena(arena);
  return arena;
}

static void ArenaRelease(Arena *arena)
{
  free(arena);
}

static void *ArenaPush(Arena *arena, u32 size, u32 align)
{
  u32 pos_aln = AlignPow2(arena->pos, align);
  u32 pos_new = pos_aln + size;
  void *result = 0;
  if(pos_new < arena->cap)
  {
    result = OffsetPtr(arena, pos_aln);
    arena->pos = pos_new;
    MemoryZero(result, size);
  }
  else
  {
    // Log to the console.
  }
  return result;
}

static void ArenaPopTo(Arena *arena, u32 pos)
{
  u32 new_pos = Clamp(ARENA_HEADER_SIZE, pos, arena->cap);
  arena->pos = new_pos;
}

static void ArenaPop(Arena *arena, u32 amt)
{
  u32 old_amt = arena->pos;
  u32 new_amt = old_amt;
  if(amt < old_amt)
  {
    new_amt -= amt;
  }
  ArenaPopTo(arena, new_amt);
}

static void ArenaClear(Arena* arena)
{
  ArenaPopTo(arena, 0);
}

static Temp TempBegin(Arena *arena)
{
  Temp temp = {arena, arena->pos};
  return temp;
}

static void TempEnd(Temp temp)
{
  Arena *arena = temp.arena;
  ArenaPopTo(arena, temp.pos);
}

static void DebugCtx_Alloc(void)
{
  u32 cap = KB(4);
  debug_arena = (Arena*)malloc(cap);
  debug_arena->pos = ARENA_HEADER_SIZE;
  debug_arena->cap = cap;
  debug_arena->name = "debug_arenas";
  debug_ctx = PushArray(debug_arena, DebugCtx, 1);
  debug_ctx->interval = 0.05f;
}

static void DebugCtx_Release(void)
{
  free(debug_arena);
}

static void DebugCtx_PushArena(Arena *arena)
{
  ArenaNode *node = PushArray(debug_arena, ArenaNode, 1);
  QueuePush(debug_ctx->first, debug_ctx->last, node);
  debug_ctx->count += 1;
  node->arena = arena;
}

static void DebugCtx_TakeSnapshot(SnapshotRing *snapshots, Arena *arena)
{
  Snapshot snap = zero_struct;
  snap.pos = arena->pos;
  snap.cap = arena->cap;

  snapshots->v[snapshots->count] = snap;
  snapshots->count = (snapshots->count + 1)%DEBUG_SNAPSHOT_CAP;
}
