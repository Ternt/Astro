// 2026-07-02


static Pool *Pool_Alloc_(PoolParams *params)
{
  U64 chunk_size = AlignPow2(params->chunk_size, KB(4));
  U64 block_size = params->block_size;
  U64 block_count = (chunk_size-POOL_HEADER_SIZE)/(sizeof(PoolBlock)+block_size);
  void *buffer = params->optional_buffer;
  if(buffer == null)
  {
    buffer = MemoryAlloc(chunk_size);
    ProfMemoryAlloc(buffer, chunk_size, "pool:0");
  }
  Pool *pool = (Pool*)buffer;
  pool->current = pool;
  pool->prev = null;
  pool->flags = params->flags;
  pool->name = params->name;
  pool->chunk_size = chunk_size;
  pool->block_size = block_size;
  pool->free_block_count = block_count;
  pool->first_block = null;
  pool->last_block = null;
  pool->free_blocks = null;
  pool->allocation_file = params->allocation_file;
  pool->allocation_line = params->allocation_line;
  return pool;
}

static void Pool_Release(Pool *pool)
{
  MemoryFree(pool);
}

static void *Pool_GetBlock(Pool *pool)
{
  Pool *current = pool->current;
  if((current->free_block_count == 0) && (current->flags & PoolFlags_Growable))
  {
    PoolParams current_params;
    current_params.flags = current->flags;
    current_params.name = current->name;
    current_params.optional_buffer = null;
    current_params.chunk_size = current->chunk_size;
    current_params.block_size = current->block_size;
    current_params.allocation_file = current->allocation_file;
    current_params.allocation_line = current->allocation_line;
    Pool *new_chunk = Pool_Alloc_(&current_params);

    StackPush_N(pool->prev, current, prev);
    current = pool->current = new_chunk;
    pool->chunk_count += 1;
  }

  void *result = null;
  if(current->free_block_count > 0)
  {
    current->free_block_count -= 1;
  }
  return result;
}

static void Pool_FreeBlock(Pool *pool, void *block)
{
}

