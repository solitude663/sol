#ifndef BASE_PLATFORM_H
#define BASE_PLATFORM_H

// NOTE(afb) :: Operating system
#define OS_WINDOWS 0
#define OS_LINUX   0
#define OS_MACOS   0

#ifdef _WIN32
#undef OS_WINDOWS
#define OS_WINDOWS 1
#endif

#ifdef  __linux__
#undef OS_LINUX
#define OS_LINUX 1
#endif

#if __APPLE__ && __MACH__
#undef OS_MACOS
#define OS_MACOS 1
#endif

#if !(OS_WINDOWS || OS_LINUX || OS_MACOS)
#error Operating system cannot be identified.
#endif

// NOTE(afb) :: Compiler
#define COMPILER_MSVC  0
#define COMPILER_GCC   0
#define COMPILER_CLANG 0

#ifdef _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#endif

#ifdef __GNUC__
#undef COMPILER_GCC
#define COMPILER_GCC 1
#endif

#ifdef __clang__
#undef COMPILER_CLANG
#define COMPILER_CLANG 1
#endif

#if !(COMPILER_GCC || COMPILER_CLANG || COMPILER_MSVC)
#error Unsupported compiler detected.
#endif

// NOTE(afb) :: Architecture


// NOTE(afb) :: Decorators
#if COMPILER_MSVC
#define per_thread __declspec(thread)
#endif
#if COMPILER_GCC
#define per_thread __thread
#endif
#if COMPILER_CLANG
#define per_thread __thread
#endif


#endif // Header guard
