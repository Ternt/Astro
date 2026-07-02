// 2026-06-14

////////////////////////////
//- Third-Party Includes

# define XXH_INLINE_ALL
# define XXH_IMPLEMENTATION
# define XXH_STATIC_LINKING_ONLY
#include "third_party/xxhash/xxhash.h"

////////////////////////////
//- Helpers

static inline F32 RandF32(F32 a, F32 b)
{
  return a + (F32)rand() / (F32)(RAND_MAX) * (b - a);
}

static inline U8 RandU8(U8 a, U8 b)
{
  return a + rand() % (b - a + 1);
}

