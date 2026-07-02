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

#if defined(_MSC_VER)
# define thread_static __declspec(thread)
#elif defined(__clang__) || defined(__GNUC__)
# define thread_static __thread
#else
# error thread_static not defined for this compiler.
#endif

#if defined(_MSC_VER)
# define force_inline __forceinline
#elif defined(__clang__) || defined(__GNUC__)
# define force_inline __attribute__((always_inline))
#else
# error force_inline not defined for this compiler.
#endif

////////////////////////////
//- Helper Type Definitions

typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef int8_t    S8;
typedef int16_t   S16;
typedef int32_t   S32;
typedef int64_t   S64;
typedef S8        B8;
typedef S16       B16;
typedef S32       B32;
typedef S64       B64;
typedef float     F32;
typedef double    F64;
typedef size_t    USize;
typedef uintptr_t UPtr;

typedef U32 Axis;
enum 
{
  Axis_X,
  Axis_Y,
  Axis_COUNT,
};

typedef U32 Direction;
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
  F32 rt;
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

#define KB(x) ((U32)(x)<<10)
#define MB(x) ((U32)(x)<<20)
#define GB(x) ((U32)(x)<<30)

#define Max(a,b)       (((a)>(b))?(a):(b))
#define Min(a,b)       (((a)<(b))?(a):(b))
#define ClampTop(a,b)  Min(a,b)
#define ClampBot(a,b)  Max(a,b)
#define Clamp(a,x,b)   (((x)<(a))?(a):(((x)>(b))?(b):(x)))
#define AlignPow2(x,a) (((x)+(a)-1)&(~((a)-1)))
#define OffsetPtr(p,o) (void*)((UPtr)(p) + (UPtr)(o))

#define ArrayCount(a)  (sizeof(a)/sizeof(a[0]))
#define InRange(x,a,b) (((x) >= (a))&&((x) <= (b)))

#define MemoryCopy(d,s,c)   memmove(d,s,c)
#define MemoryMatch(a,b,sz) (memcmp(a,b,sz)==0)
#define MemoryZero(p,sz)    memset(p,0,sz)
#define MemoryZeroArray(a)  MemoryZero(a,sizeof(a))
#define MemoryZeroStruct(s) MemoryZero(s,sizeof(*(s)))
#define MemoryAlloc(sz)     malloc(sz)
#define MemoryFree(ptr)     free(ptr)

#define EachIdx(it, min, max, inc) (U32 it = (min); it < (max); it += (inc))

////////////////////////////////
//- Linked List Builder Pointer Macros

#define CheckPtrNil(p,nil) ((p)==(nil))
#define SetPtrNil(p,nil) ((p)=(nil))

#define DLLInsert_NPZ(f,l,p,n,next,prev,nil) \
(CheckPtrNil(f,(nil)) ? (((f) = (l) = (n)), SetPtrNil((n)->next,(nil)), SetPtrNil((n)->prev,(nil))) :\
CheckPtrNil(p,(nil)) ? (SetPtrNil((n)->prev,(nil)), (n)->next = (f), (CheckPtrNil(f,(nil)) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((CheckPtrNil((p)->next,(nil)) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,nil) DLLInsert_NPZ(f,l,l,n,next,prev,nil)
#define DLLRemove_NPZ(f,l,n,next,prev,nil) (((f)==(n))?\
((f)=(f)->next, (CheckPtrNil(f,(nil)) ? (SetPtrNil(l,(nil))) : SetPtrNil((f)->prev,(nil)))):\
((l)==(n))?\
((l)=(l)->prev, (CheckPtrNil(l,(nil)) ? (SetPtrNil(f,(nil))) : SetPtrNil((l)->next,(nil)))):\
((CheckPtrNil((n)->next,(nil)) ? (0) : ((n)->next->prev=(n)->prev)),\
(CheckPtrNil((n)->prev,(nil)) ? (0) : ((n)->prev->next=(n)->next))))
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(f,l,n,next,prev,0)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushBack_NPZ(l,f,n,prev,next,0)
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(f,l,p,n,next,prev,0)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(f,l,n,next,prev,0)

#define QueuePush_NZ(f,l,n,next,nil) (CheckPtrNil(f,(nil))?\
(((f)=(l)=(n)), SetPtrNil((n)->next,(nil))):\
((l)->next=(n),(l)=(n),SetPtrNil((n)->next,(nil))))
#define QueuePushFront_NZ(f,l,n,next,nil) (CheckPtrNil(f,(nil)) ? (((f) = (l) = (n)), SetPtrNil((n)->next,(nil))) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,nil) ((f)==(l)?\
(SetPtrNil(f,(nil)),SetPtrNil(l,(nil))):\
((f)=(f)->next))
#define QueuePush_N(f,l,n,next) QueuePush_NZ(f,l,n,next,0)
#define QueuePushFront_N(f,l,n,next) QueuePushFront_NZ(f,l,n,next,0)
#define QueuePop_N(f,l,next) QueuePop_NZ(f,l,next,0)

#define StackPush_N(f,n,next)    ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,nil)  (CheckPtrNil(f,(nil))?0:((f)=(f)->next))
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

////////////////////////////////
//- Linked List Builder Index Macros

#define IndexNode(nodes,i) (&nodes[i])
#define CheckIndexNil(i,nil) ((i)==(nil))
#define SetIndexNil(i,nil) ((i)==(nil))

#define DLLIndexInsert_NPZ(nodes,f,l,p,n,next,prev,nil) \
(CheckIndexNil(f,(nil)) ? (((f) = (l) = (n)), SetIndexNil(IndexNode(nodes,n)->next,(nil)), SetIndexNil(IndexNode(nodes,n)->prev,(nil))) :\
CheckIndexNil(p,(nil)) ? (SetIndexNil(IndexNode(nodes,n)->prev,(nil)), IndexNode(nodes,n)->next = (f), (CheckIndexNil(f,(nil)) ? (0) : (IndexNode(nodes,f)->prev = (n))), (f) = (n)) :\
((CheckIndexNil(IndexNode(nodes,p)->next,(nil)) ? (0) : ((IndexNode(nodes,IndexNode(nodes,p)->next)->prev) = (n))), IndexNode(nodes,n)->next = IndexNode(nodes,p)->next, IndexNode(nodes,n)->prev = (p), IndexNode(nodes,p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLIndexPushBack_NPZ(list,f,l,n,next,prev,nil) DLLIndexInsert_NPZ(list,f,l,l,n,next,prev,nil)
#define DLLIndexRemove_NPZ(nodes,f,l,n,next,prev,nil) (((f)==(n))?\
((f)=IndexNode(nodes,f)->next, (CheckIndexNil(f,(nil)) ? (SetIndexNil(l,(nil))) : SetIndexNil(IndexNode(nodes,f)->prev,(nil)))):\
((l)==(n))?\
((l)=IndexNode(nodes,l)->prev, (CheckIndexNil(l,(nil)) ? (SetIndexNil(f,(nil))) : SetIndexNil(IndexNode(nodes,l)->next,(nil)))):\
((CheckIndexNil(IndexNode(nodes,n)->next,(nil)) ? (0) : (IndexNode(nodes,IndexNode(nodes,n)->next)->prev=IndexNode(nodes,n)->prev)),\
(CheckIndexNil(IndexNode(nodes,n)->prev,(nil)) ? (0) : (IndexNode(nodes,IndexNode(nodes,n)->prev)->next=IndexNode(nodes,n)->next))))
#define DLLIndexPushBack_Z(list,f,l,n,nil) DLLIndexPushBack_NPZ(list,f,l,n,next,prev,nil)
#define DLLIndexPushFront_Z(list,f,l,n,nil) DLLIndexPushBack_NPZ(list,l,f,n,prev,next,nil)
#define DLLIndexRemove_Z(list,f,l,n,nil) DLLIndexRemove_NPZ(list,l,f,n,next,prev,nil)

////////////////////////////
//- Helper Global Constants

#define max_u8  (U8)0xff
#define max_u16 (U16)0xffff
#define max_u32 (U32)0xffffffff
#define max_u64 (U64)0xffffffffffffffffllu

////////////////////////////
//- Helper Functions

static inline F32 RandF32(F32 a, F32 b);
static inline U8  RandU8(U8 a, U8 b);

#endif // HELPERS_H
