/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

#ifndef MU_TYPES
#define MU_TYPES

#include <mu/platform.h>

#if MU_COMPILER_MSVC || MU_COMPILER_MSVC_COMPATIBLE
#include <intrin.h>
#else
#include <immintrin.h>
#endif

#include <cstdint>
#include <cstddef>
#include <cstdio>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32_t bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef size_t uintptr;
typedef ptrdiff_t intptr;

#endif
