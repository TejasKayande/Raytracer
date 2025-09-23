#pragma once

#include <cstdint>
#include <string>
#include <iostream>

// Figuring out the Operating System
#if defined(_WIN32)
#  define ON_WINDOWS 1
#elif defined(__linux__)
#  define ON_LINUX 1
#endif

// Figuring out the Compiler
#if defined(__clang__)
#  define CLANG_COMPILER 1
#elif defined(_MSC_VER)
#  define CL_COMPILER 1
#endif

// Resetting the rest of the Macros to 0
#if !defined(ON_WINDOWS)
#  define ON_WINDOWS 0
#endif
#if !defined(ON_LINUX)
#  define ON_LINUX 0
#endif
#if !defined(CLANG_COMPILER)
#  define CLANG_COMPILER 0
#endif
#if !defined(CL_COMPILER)
#  define CL_COMPILER 0
#endif

#define KB(x)   ((unsigned long long)(x) * 1024ULL)
#define MB(x)   (KB(x) * 1024ULL)
#define GB(x)   (MB(x) * 1024ULL)

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(*(x)))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

using  i8 = int8_t;
using  u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

namespace Base {

    struct RGBA {
        u8 r, g, b, a;
    };

    using Color = u32;

} // namespace Base

#define global   static 
#define persist  static 
#define internal static 
