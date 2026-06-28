// 2026-06-14

#ifndef HELPERS_H
#define HELPERS_H

////////////////////////////
//- Helper CLib Includes

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

////////////////////////////
//- Helper Keywords

#define null (0)
#define zero_struct {0}

#ifndef true
#define true 1
#endif
#ifndef false 
#define false 0
#endif

////////////////////////////
//- Helper Type Definitions

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;
typedef s8        b8;
typedef s16       b16;
typedef s32       b32;
typedef s64       b64;
typedef float     f32;
typedef double    f64;
typedef size_t    usize;
typedef uintptr_t uptr;

typedef u32 Axis;
enum 
{
  Axis_X,
  Axis_Y,
  Axis_COUNT,
};

typedef u32 Direction;
enum
{
  Direction_Null  = -1,
  Direction_North = 0,
  Direction_South = 1,
  Direction_West  = 2,
  Direction_East  = 3
};

typedef struct Xform {
  Vector2 tr;
  f32 rt;
  Vector2 sc;
} Xform;

////////////////////////////
//- Helper Intrinsics

#if defined(_MSC_VER)
# define AlignOf(T) __alignof(T)
#elif defined(__clang__) 
# define AlignOf(T) __alignof(T)
#elif defined(__GNUC__)
# define AlignOf(T) __alignof__(T)
#else
# error AlignOf not available for this compiler.
#endif

#if defined(_MSC_VER)
# define force_inline __forceinline
#elif defined(__clang__) || defined(__GNUC__)
# define force_inline __attribute__((always_inline))
#else
# error force_inline not available for this compiler.
#endif

// Debug Break Intrinsic
#if defined(_MSC_VER)
# define Trap() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
# define Trap() __builtin_trap()
#else
# error Trap not available for this compiler.
#endif

// Assertions
#define AssertAlways(x) do{if(!(x)){ Trap(); }}while(0);
#if DEBUG
#define Assert(x) AssertAlways(x)
#else
#define Assert(x) (void)(x)
#endif
#define StaticAssert(C,ID) global U8_t Glue(ID, __LINE__)[(!!(C))*2-1]

////////////////////////////
//- Helper Macros

#define KB(x) ((u32)(x)<<10)
#define MB(x) ((u32)(x)<<20)
#define GB(x) ((u32)(x)<<30)

#define Max(a,b)       (((a)>(b))?(a):(b))
#define Min(a,b)       (((a)<(b))?(a):(b))
#define ClampTop(a,b)  Min(a,b)
#define ClampBot(a,b)  Max(a,b)
#define Clamp(a,x,b)   (((x)<(a))?(a):(((x)>(b))?(b):(x)))
#define AlignPow2(x,a) (((x)+(a)-1)&(~((a)-1)))
#define OffsetPtr(p,o) (void*)((uptr)(p) + (uptr)(o))

#define ArrayCount(a)  (sizeof(a)/sizeof(a[0]))
#define InRange(x,a,b) (((x) >= (a))&&((x) <= (b)))

#define MemoryCopy(d,s,c)   memmove(d,s,c)
#define MemoryMatch(a,b,sz) (memcmp(a,b,sz)==0)
#define MemoryZero(p,sz)    memset(p,0,sz)
#define MemoryZeroArray(a)  MemoryZero(a,sizeof(a))
#define MemoryZeroStruct(s) MemoryZero(s,sizeof(*(s)))

#define EachIdx(it, min, max, inc) (u32 it = (min); it < (max); it += (inc))

////////////////////////////////
//- Linked List Builder Macros

#define CheckNil(p,nil) ((p)==(nil))
#define SetNil(p,nil) ((p)=(nil))

#define DLLInsert_NPZ(f,l,p,n,next,prev,nil) \
(CheckNil(f,(nil)) ? (((f) = (l) = (n)), SetNil((n)->next,(nil)), SetNil((n)->prev,(nil))) :\
CheckNil(p,(nil)) ? (SetNil((n)->prev,(nil)), (n)->next = (f), (CheckNil(f,(nil)) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((CheckNil((p)->next,(nil)) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,nil) DLLInsert_NPZ(f,l,l,n,next,prev,nil)
#define DLLRemove_NPZ(f,l,n,next,prev,nil) (((f)==(n))?\
((f)=(f)->next, (CheckNil(f,(nil)) ? (SetNil(l,(nil))) : SetNil((f)->prev,(nil)))):\
((l)==(n))?\
((l)=(l)->prev, (CheckNil(l,(nil)) ? (SetNil(f,(nil))) : SetNil((l)->next,(nil)))):\
((CheckNil((n)->next,(nil)) ? (0) : ((n)->next->prev=(n)->prev)),\
(CheckNil((n)->prev,(nil)) ? (0) : ((n)->prev->next=(n)->next))))
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(f,l,n,next,prev,0)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushBack_NPZ(l,f,n,prev,next,0)
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(f,l,p,n,next,prev,0)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(f,l,n,next,prev,0)

#define QueuePush_NZ(f,l,n,next,nil) (CheckNil(f,(nil))?\
(((f)=(l)=(n)), SetNil((n)->next,(nil))):\
((l)->next=(n),(l)=(n),SetNil((n)->next,(nil))))
#define QueuePushFront_NZ(f,l,n,next,nil) (CheckNil(f,(nil)) ? (((f) = (l) = (n)), SetNil((n)->next,(nil))) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,nil) ((f)==(l)?\
(SetNil(f,(nil)),SetNil(l,(nil))):\
((f)=(f)->next))
#define QueuePush_N(f,l,n,next) QueuePush_NZ(f,l,n,next,0)
#define QueuePushFront_N(f,l,n,next) QueuePushFront_NZ(f,l,n,next,0)
#define QueuePop_N(f,l,next) QueuePop_NZ(f,l,next,0)

#define StackPush_N(f,n,next)    ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,nil)  (CheckNil(f,(nil))?0:((f)=(f)->next))
#define StackPop_N(f,next)       StackPop_NZ(f,next,0)

#define DLLPushBack(f,l,n)       DLLPushBack_NP(f,l,n,next,prev)
#define DLLPushFront(f,l,n)      DLLPushBack_NP(l,f,n,prev,next)
#define DLLInsert(f,l,p,n)       DLLInsert_NP(f,l,p,n,next,prev)
#define DLLRemove(f,l,n)         DLLRemove_NP(f,l,n,next,prev)
#define QueuePush(f,l,n)         QueuePush_N(f,l,n,next)
#define QueuePushFront(f,l,n)    QueuePushFront_N(f,l,n,next)
#define QueuePop(f,l)            QueuePop_N(f,l,next)
#define StackPush(f,n)           StackPush_N(f,n,next)
#define StackPop(f)              StackPop_N(f,next)

////////////////////////////
//- Helper Global Constants

#define max_u8  (u8)0xff
#define max_u16 (u16)0xffff
#define max_u32 (u32)0xffffffff
#define max_u64 (u64)0xffffffffffffffffllu

////////////////////////////
//- Helper Functions

static inline Vector2 WorldToScreen(Vector2 v);
static inline f32 RandF32(f32 a, f32 b);
static inline u8 RandU8(u8 a, u8 b);

#endif // HELPERS_H
