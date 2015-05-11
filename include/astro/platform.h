/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_PLATFORM
#define ASTRO_PLATFORM

/** Platform detection heavily inspired by bx (https://github.com/bkaradzic/bx/blob/master/include/bx/platform.h) */
#define ASTRO_COMPILER_CLANG           0
#define ASTRO_COMPILER_CLANG_ANALYZER  0
#define ASTRO_COMPILER_GCC             0
#define ASTRO_COMPILER_MSVC            0
#define ASTRO_COMPILER_MSVC_COMPATIBLE 0

#define ASTRO_PLATFORM_ANDROID     0
#define ASTRO_PLATFORM_EMSCRIPTEN  0
#define ASTRO_PLATFORM_FREEBSD     0
#define ASTRO_PLATFORM_IOS         0
#define ASTRO_PLATFORM_LINUX       0
#define ASTRO_PLATFORM_NACL        0
#define ASTRO_PLATFORM_OSX         0
#define ASTRO_PLATFORM_RPI         0
#define ASTRO_PLATFORM_WIN32       0
#define ASTRO_PLATFORM_WINRT       0
#define ASTRO_PLATFORM_WINDOWS     0

#define ASTRO_CPU_ARM          0
#define ASTRO_CPU_JIT          0
#define ASTRO_CPU_MIPS         0
#define ASTRO_CPU_PPC          0
#define ASTRO_CPU_X86          0

#define ASTRO_ARCH_32BIT       0
#define ASTRO_ARCH_64BIT       0

#define ASTRO_ENDIAN_BIG       0
#define ASTRO_ENDIAN_LITTLE    0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
# undef  ASTRO_COMPILER_CLANG
# define ASTRO_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
# if defined(__clang_analyzer__)
#   undef  ASTRO_COMPILER_CLANG_ANALYZER
#   define ASTRO_COMPILER_CLANG_ANALYZER 1
# endif // defined(__clang_analyzer__)
# if defined(_MSC_VER)
#   undef  ASTRO_COMPILER_MSVC_COMPATIBLE
#   define ASTRO_COMPILER_MSVC_COMPATIBLE _MSC_VER
# endif // defined(_MSC_VER)
#elif defined(_MSC_VER)
# undef  ASTRO_COMPILER_MSVC
# define ASTRO_COMPILER_MSVC _MSC_VER
# undef  ASTRO_COMPILER_MSVC_COMPATIBLE
# define ASTRO_COMPILER_MSVC_COMPATIBLE _MSC_VER
#elif defined(__GNUC__)
# undef  ASTRO_COMPILER_GCC
# define ASTRO_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
# error "ASTRO_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     || \
  defined(__aarch64__) || \
  defined(_M_ARM)
# undef  ASTRO_CPU_ARM
# define ASTRO_CPU_ARM 1
# define ASTRO_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     || \
    defined(__mips_isa_rev) || \
    defined(__mips64)
# undef  ASTRO_CPU_MIPS
# define ASTRO_CPU_MIPS 1
# define ASTRO_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        || \
    defined(__powerpc__)   || \
    defined(__powerpc64__)
# undef  ASTRO_CPU_PPC
# define ASTRO_CPU_PPC 1
# define ASTRO_CACHE_LINE_SIZE 128
#elif defined(_M_IX86)    || \
    defined(_M_X64)     || \
    defined(__i386__)   || \
    defined(__x86_64__)
# undef  ASTRO_CPU_X86
# define ASTRO_CPU_X86 1
# define ASTRO_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
# undef  ASTRO_CPU_JIT
# define ASTRO_CPU_JIT 1
# define ASTRO_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    || \
  defined(_M_X64)        || \
  defined(__aarch64__)   || \
  defined(__64BIT__)     || \
  defined(__mips64)      || \
  defined(__powerpc64__) || \
  defined(__ppc64__)
# undef  ASTRO_ARCH_64BIT
# define ASTRO_ARCH_64BIT 64
#else
# undef  ASTRO_ARCH_32BIT
# define ASTRO_ARCH_32BIT 32
#endif //

#if ASTRO_CPU_PPC
# undef  ASTRO_ENDIAN_BIG
# define ASTRO_ENDIAN_BIG 1
#else
# undef  ASTRO_ENDIAN_LITTLE
# define ASTRO_ENDIAN_LITTLE 1
#endif // ASTRO_PLATFORM_

#if defined(_WIN32) || defined(_WIN64)
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
# ifndef NOMINMAX
#   define NOMINMAX
# endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset.
# if defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#   include <winapifamily.h>
# endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
# if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#   undef  ASTRO_PLATFORM_WINDOWS
#   if !defined(WINVER) && !defined(_WIN32_WINNT)
#     if ASTRO_ARCH_64BIT
//        When building 64-bit target Win7 and above.
#       define WINVER 0x0601
#       define _WIN32_WINNT 0x0601
#     else
//        Windows Server 2003 with SP1, Windows XP with SP2 and above
#       define WINVER 0x0502
#       define _WIN32_WINNT 0x0502
#     endif // ASTRO_ARCH_64BIT
#   endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#   define ASTRO_PLATFORM_WINDOWS _WIN32_WINNT
#   undef  ASTRO_PLATFORM_WIN32
#   define  ASTRO_PLATFORM_WIN32 1
# else
#   undef  ASTRO_PLATFORM_WINRT
#   define ASTRO_PLATFORM_WINRT 1
# endif
#elif defined(__VCASTROVER__)
// RaspberryPi compiler defines __linux__
# undef  ASTRO_PLATFORM_RPI
# define ASTRO_PLATFORM_RPI 1
#elif defined(__native_client__)
// NaCl compiler defines __linux__
# include <ppapi/c/pp_macros.h>
# undef  ASTRO_PLATFORM_NACL
# define ASTRO_PLATFORM_NACL PPAPI_RELEASE
#elif defined(__ANDROID__)
// Android compiler defines __linux__
# include <android/api-level.h>
# undef  ASTRO_PLATFORM_ANDROID
# define ASTRO_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__linux__)
# undef  ASTRO_PLATFORM_LINUX
# define ASTRO_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
# undef  ASTRO_PLATFORM_IOS
# define ASTRO_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
# undef  ASTRO_PLATFORM_OSX
# define ASTRO_PLATFORM_OSX 1
#elif defined(EMSCRIPTEN)
# undef  ASTRO_PLATFORM_EMSCRIPTEN
# define ASTRO_PLATFORM_EMSCRIPTEN 1
#elif defined(__FreeBSD__)
# undef  ASTRO_PLATFORM_FREEBSD
# define ASTRO_PLATFORM_FREEBSD 1
#else
# error "ASTRO_PLATFORM_* is not defined!"
#endif //

#define ASTRO_PLATFORM_POSIX (0 \
            || ASTRO_PLATFORM_ANDROID \
            || ASTRO_PLATFORM_EMSCRIPTEN \
            || ASTRO_PLATFORM_FREEBSD \
            || ASTRO_PLATFORM_IOS \
            || ASTRO_PLATFORM_LINUX \
            || ASTRO_PLATFORM_NACL \
            || ASTRO_PLATFORM_OSX \
            || ASTRO_PLATFORM_RPI \
            )

#if ASTRO_COMPILER_GCC
# define ASTRO_COMPILER_NAME "GCC " \
        ASTRO_STRINGIZE(__GNUC__) "." \
        ASTRO_STRINGIZE(__GNUC_MINOR__) "." \
        ASTRO_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif ASTRO_COMPILER_CLANG
# define ASTRO_COMPILER_NAME "Clang " \
        ASTRO_STRINGIZE(__clang_major__) "." \
        ASTRO_STRINGIZE(__clang_minor__) "." \
        ASTRO_STRINGIZE(__clang_patchlevel__)
#elif ASTRO_COMPILER_MSVC
# if ASTRO_COMPILER_MSVC >= 1900
#   define ASTRO_COMPILER_NAME "MSVC 14.0"
# elif ASTRO_COMPILER_MSVC >= 1800
#   define ASTRO_COMPILER_NAME "MSVC 12.0"
# elif ASTRO_COMPILER_MSVC >= 1700
#   define ASTRO_COMPILER_NAME "MSVC 11.0"
# elif ASTRO_COMPILER_MSVC >= 1600
#   define ASTRO_COMPILER_NAME "MSVC 10.0"
# elif ASTRO_COMPILER_MSVC >= 1500
#   define ASTRO_COMPILER_NAME "MSVC 9.0"
# else
#   define ASTRO_COMPILER_NAME "MSVC"
# endif //
#endif // ASTRO_COMPILER_

#if ASTRO_PLATFORM_ANDROID
# define ASTRO_PLATFORM_NAME "Android " \
        ASTRO_STRINGIZE(ASTRO_PLATFORM_ANDROID)
#elif ASTRO_PLATFORM_EMSCRIPTEN
# define ASTRO_PLATFORM_NAME "asm.js " \
        ASTRO_STRINGIZE(__EMSCRIPTEN_major__) "." \
        ASTRO_STRINGIZE(__EMSCRIPTEN_minor__) "." \
        ASTRO_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif ASTRO_PLATFORM_FREEBSD
# define ASTRO_PLATFORM_NAME "FreeBSD"
#elif ASTRO_PLATFORM_IOS
# define ASTRO_PLATFORM_NAME "iOS"
#elif ASTRO_PLATFORM_LINUX
# define ASTRO_PLATFORM_NAME "Linux"
#elif ASTRO_PLATFORM_NACL
# define ASTRO_PLATFORM_NAME "NaCl " \
        ASTRO_STRINGIZE(ASTRO_PLATFORM_NACL)
#elif ASTRO_PLATFORM_OSX
# define ASTRO_PLATFORM_NAME "OSX"
#elif ASTRO_PLATFORM_PS4
# define ASTRO_PLATFORM_NAME "PlayStation 4"
#elif ASTRO_PLATFORM_QNX
# define ASTRO_PLATFORM_NAME "QNX"
#elif ASTRO_PLATFORM_RPI
# define ASTRO_PLATFORM_NAME "RaspberryPi"
#elif ASTRO_PLATFORM_WINDOWS
# define ASTRO_PLATFORM_NAME "Windows"
#elif ASTRO_PLATFORM_WINRT
# define ASTRO_PLATFORM_NAME "WinRT"
#elif ASTRO_PLATFORM_XBOX360
# define ASTRO_PLATFORM_NAME "Xbox 360"
#elif ASTRO_PLATFORM_XBOXONE
# define ASTRO_PLATFORM_NAME "Xbox One"
#endif // ASTRO_PLATFORM_

#if ASTRO_CPU_ARM
# define ASTRO_CPU_NAME "ARM"
#elif ASTRO_CPU_MIPS
# define ASTRO_CPU_NAME "MIPS"
#elif ASTRO_CPU_PPC
# define ASTRO_CPU_NAME "PowerPC"
#elif ASTRO_CPU_JIT
# define ASTRO_CPU_NAME "JIT-VM"
#elif ASTRO_CPU_X86
# define ASTRO_CPU_NAME "x86"
#endif // ASTRO_CPU_

#if ASTRO_ARCH_32BIT
# define ASTRO_ARCH_NAME "32-bit"
#elif ASTRO_ARCH_64BIT
# define ASTRO_ARCH_NAME "64-bit"
#endif // ASTRO_ARCH_

#endif
