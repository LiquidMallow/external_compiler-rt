/*===- InstrProfilingPort.h- Support library for PGO instrumentation ------===*\
|*
|*                     The LLVM Compiler Infrastructure
|*
|* This file is distributed under the University of Illinois Open Source
|* License. See LICENSE.TXT for details.
|*
\*===----------------------------------------------------------------------===*/

#ifndef PROFILE_INSTRPROFILING_PORT_H_
#define PROFILE_INSTRPROFILING_PORT_H_

#ifdef _MSC_VER
#define COMPILER_RT_ALIGNAS(x) __declspec(align(x))
#define COMPILER_RT_VISIBILITY
/* FIXME: selectany does not have the same semantics as weak. */
#define COMPILER_RT_WEAK __declspec(selectany)
/* Need to include <windows.h> */
#define COMPILER_RT_ALLOCA _alloca
/* Need to include <stdio.h> and <io.h> */
#define COMPILER_RT_FTRUNCATE(f,l) _chsize(_fileno(f),l)
#elif __GNUC__
#define COMPILER_RT_ALIGNAS(x) __attribute__((aligned(x)))
#define COMPILER_RT_VISIBILITY __attribute__((visibility("hidden")))
#define COMPILER_RT_WEAK __attribute__((weak))
#define COMPILER_RT_ALLOCA __builtin_alloca
#define COMPILER_RT_FTRUNCATE(f,l) ftruncate(fileno(f),l)
#endif

#if defined(__APPLE__)
#define COMPILER_RT_SEG "__DATA,"
#else
#define COMPILER_RT_SEG ""
#endif

#ifdef _MSC_VER
#define COMPILER_RT_SECTION(Sect) __declspec(allocate(Sect))
#else
#define COMPILER_RT_SECTION(Sect) __attribute__((section(Sect)))
#endif

#define COMPILER_RT_MAX_HOSTLEN 128
#ifdef _MSC_VER
#define COMPILER_RT_GETHOSTNAME(Name, Len) gethostname(Name, Len)
#elif defined(__ORBIS__)
#define COMPILER_RT_GETHOSTNAME(Name, Len) ((void)(Name), (void)(Len), (-1))
#else
#define COMPILER_RT_GETHOSTNAME(Name, Len) lprofGetHostName(Name, Len)
#define COMPILER_RT_HAS_UNAME 1
#endif

#if COMPILER_RT_HAS_ATOMICS == 1
#ifdef _MSC_VER
#include <windows.h>
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#if defined(_WIN64)
#define COMPILER_RT_BOOL_CMPXCHG(Ptr, OldV, NewV)                              \
  (InterlockedCompareExchange64((LONGLONG volatile *)Ptr, (LONGLONG)NewV,      \
                                (LONGLONG)OldV) == (LONGLONG)OldV)
#define COMPILER_RT_PTR_FETCH_ADD(DomType, PtrVar, PtrIncr)                    \
  (DomType *)InterlockedExchangeAdd64((LONGLONG volatile *)&PtrVar,            \
                                      (LONGLONG)sizeof(DomType) * PtrIncr)
#else /* !defined(_WIN64) */
#define COMPILER_RT_BOOL_CMPXCHG(Ptr, OldV, NewV)                              \
  (InterlockedCompareExchange((LONG volatile *)Ptr, (LONG)NewV, (LONG)OldV) == \
   (LONG)OldV)
#define COMPILER_RT_PTR_FETCH_ADD(DomType, PtrVar, PtrIncr)                    \
  (DomType *)InterlockedExchangeAdd((LONG volatile *)&PtrVar,                  \
                                    (LONG)sizeof(DomType) * PtrIncr)
#endif
#else /* !defined(_MSC_VER) */
#define COMPILER_RT_BOOL_CMPXCHG(Ptr, OldV, NewV)                              \
  __sync_bool_compare_and_swap(Ptr, OldV, NewV)
#define COMPILER_RT_PTR_FETCH_ADD(DomType, PtrVar, PtrIncr)                    \
  (DomType *)__sync_fetch_and_add((long *)&PtrVar, sizeof(DomType) * PtrIncr)
#endif
#else /* COMPILER_RT_HAS_ATOMICS != 1 */
#include "InstrProfilingUtil.h"
#define COMPILER_RT_BOOL_CMPXCHG(Ptr, OldV, NewV)                              \
  lprofBoolCmpXchg((void **)Ptr, OldV, NewV)
#define COMPILER_RT_PTR_FETCH_ADD(DomType, PtrVar, PtrIncr)                    \
  (DomType *)lprofPtrFetchAdd((void **)&PtrVar, sizeof(DomType) * PtrIncr)
#endif

#define PROF_ERR(Format, ...)                                                  \
  fprintf(stderr, "LLVM Profile Error: " Format, __VA_ARGS__);

#define PROF_WARN(Format, ...)                                                 \
  fprintf(stderr, "LLVM Profile Warning: " Format, __VA_ARGS__);

#define PROF_NOTE(Format, ...)                                                 \
  fprintf(stderr, "LLVM Profile Note: " Format, __VA_ARGS__);

#if defined(__FreeBSD__)

#include <inttypes.h>
#include <sys/types.h>

#else /* defined(__FreeBSD__) */

#include <inttypes.h>
#include <stdint.h>

#endif /* defined(__FreeBSD__) && defined(__i386__) */

#endif /* PROFILE_INSTRPROFILING_PORT_H_ */
