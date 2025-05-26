#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef float  f32;
typedef double f64;

typedef i8  b8;
typedef i16 b16;
typedef i32 b32;

typedef uintptr_t ptr_value;
typedef intptr_t  sptr_value;

#define internal		 static
#define local_persist	 static
#define global			 static

#define KB(amount) (u64)((amount) * 1024)
#define MB(amount) (u64)((KB(amount)) * 1024)
#define GB(amount) (u64)((MB(amount)) * 1024)
#define TB(amount) (u64)((GB(amount)) * 1024)

#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))

#define Max(a, b) a > b ? a : b;
#define Min(a, b) a < b ? a : b;

#if OS_WINDOWS
#define Debug_Break() __debugbreak()
#else
#define Debug_Break() (*(volatile int*)0 = 1)
#endif

// #undef Assert
#if BUILD_DEBUG
#define Assert(x) do { if(!(x)) { Debug_Break(); } } while(0)
#else
#define Assert(x) ((void)(x))
#endif

#ifndef MemorySet
#define MemorySet(mem, size, value) memset(mem, value, size)
#endif
#ifndef MemoryMove
#define MemoryMove(dest, src, size) memmove(dest, src, size)
#endif
#ifndef MemoryCopy
#define MemoryCopy(dest, src, size) memcpy(dest, src, size)
#endif

#define UnusedVariable(x) (void)x;

#define FNV1A_PRIME64 0x00000100000001b3ull
#define FNV1A_PRIME32 0x01000193

#define FNV1A_OFFSET64 0xcbf29ce484222325ull
#define FNV1A_OFFSET32 0x811c9dc5

internal u64 FNV1A64(void* data, u64 size);
internal u32 FNV1A32(void* data, u64 size);

// TODO(afb) :: Murmur hash https://en.wikipedia.org/wiki/MurmurHash

#endif // Header guard
