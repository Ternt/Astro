// 2026-06-19

#ifndef PROFILING_H
#define PROFILING_H

////////////////////////////
//- Profiling Macros

#ifndef PROFILE_TRACY
#define PROFILE_TRACY 0
#endif

////////////////////////////
//- Tracy Profiling Includes

#if PROFILE_TRACY
# define TRACY_ENABLE 1
# include "third_party/tracy/tracy.h"
# pragma comment(lib, "TracyClient.lib")
#endif

////////////////////////////
//- Tracy Profiling Macros

#if PROFILE_TRACY
# define ProfBegin() TracyCZoneN(ctx, __func__, 1)
# define ProfBeginN(name) TracyCZoneN(ctx, name, 1)
# define ProfBeginNC(name, color) TracyCZoneNC(ctx, name, color, 1)
# define ProfEnd() TracyCZoneEnd(ctx)
# define ProfScopeBegin() 
# define ProfScopeEnd() TracyCFrameMark
# define ProfMemoryAlloc(ptr, sz, name) TracyCAllocN(ptr, sz, name)
# define ProfMemoryFree(ptr, name) TracyCFreeN(ptr, name)
#endif

////////////////////////////
//- Stub Profiling Macros

#if !defined(ProfBegin)
# define ProfBegin() (void)(0)
# define ProfBeginN(name) (void)(0)
# define ProfBeginNC(name,color) (void)(0)
# define ProfEnd() (void)(0)
# define ProfScopeBegin() (void)(0) 
# define ProfScopeEnd() (void)(0)
# define ProfMemoryAlloc(ptr, sz, name) (void)(0)
# define ProfMemoryFree(ptr, name) (void)(0)
#endif

#endif // PROFILING_H
