/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

#ifndef MU_PLATFORM
#define MU_PLATFORM

/** Platform detection heavily inspired by bx (https://github.com/bkaradzic/bx/blob/master/include/bx/platform.h) */
#define MU_COMPILER_CLANG           0
#define MU_COMPILER_CLANG_ANALYZER  0
#define MU_COMPILER_GCC             0
#define MU_COMPILER_MSVC            0
#define MU_COMPILER_MSVC_COMPATIBLE 0

#define MU_PLATFORM_ANDROID     0
#define MU_PLATFORM_EMSCRIPTEN  0
#define MU_PLATFORM_FREEBSD     0
#define MU_PLATFORM_IOS         0
#define MU_PLATFORM_LINUX       0
#define MU_PLATFORM_NACL        0
#define MU_PLATFORM_OSX         0
#define MU_PLATFORM_RPI         0
#define MU_PLATFORM_WIN32       0
#define MU_PLATFORM_WINRT       0

#define MU_CPU_ARM          0
#define MU_CPU_JIT          0
#define MU_CPU_MIPS         0
#define MU_CPU_PPC          0
#define MU_CPU_X86          0

#define MU_ARCH_32BIT       0
#define MU_ARCH_64BIT       0

#define MU_ENDIAN_BIG       0
#define MU_ENDIAN_LITTLE    0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
# undef  MU_COMPILER_CLANG
# define MU_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
# if defined(__clang_analyzer__)
#   undef  MU_COMPILER_CLANG_ANALYZER
#   define MU_COMPILER_CLANG_ANALYZER 1
# endif // defined(__clang_analyzer__)
# if defined(_MSC_VER)
#   undef  MU_COMPILER_MSVC_COMPATIBLE
#   define MU_COMPILER_MSVC_COMPATIBLE _MSC_VER
# endif // defined(_MSC_VER)
#elif defined(_MSC_VER)
# undef  MU_COMPILER_MSVC
# define MU_COMPILER_MSVC _MSC_VER
# undef  MU_COMPILER_MSVC_COMPATIBLE
# define MU_COMPILER_MSVC_COMPATIBLE _MSC_VER
#elif defined(__GNUC__)
# undef  MU_COMPILER_GCC
# define MU_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
# error "MU_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     || \
  defined(__aarch64__) || \
  defined(_M_ARM)
# undef  MU_CPU_ARM
# define MU_CPU_ARM 1
# define MU_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     || \
    defined(__mips_isa_rev) || \
    defined(__mips64)
# undef  MU_CPU_MIPS
# define MU_CPU_MIPS 1
# define MU_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        || \
    defined(__powerpc__)   || \
    defined(__powerpc64__)
# undef  MU_CPU_PPC
# define MU_CPU_PPC 1
# define MU_CACHE_LINE_SIZE 128
#elif defined(_M_IX86)    || \
    defined(_M_X64)     || \
    defined(__i386__)   || \
    defined(__x86_64__)
# undef  MU_CPU_X86
# define MU_CPU_X86 1
# define MU_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
# undef  MU_CPU_JIT
# define MU_CPU_JIT 1
# define MU_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    || \
  defined(_M_X64)        || \
  defined(__aarch64__)   || \
  defined(__64BIT__)     || \
  defined(__mips64)      || \
  defined(__powerpc64__) || \
  defined(__ppc64__)
# undef  MU_ARCH_64BIT
# define MU_ARCH_64BIT 64
#else
# undef  MU_ARCH_32BIT
# define MU_ARCH_32BIT 32
#endif //

#if MU_CPU_PPC
# undef  MU_ENDIAN_BIG
# define MU_ENDIAN_BIG 1
#else
# undef  MU_ENDIAN_LITTLE
# define MU_ENDIAN_LITTLE 1
#endif // MU_PLATFORM_

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
#   undef  MU_PLATFORM_WINDOWS
#   if !defined(WINVER) && !defined(_WIN32_WINNT)
#     if MU_ARCH_64BIT
//        When building 64-bit target Win7 and above.
#       define WINVER 0x0601
#       define _WIN32_WINNT 0x0601
#     else
//        Windows Server 2003 with SP1, Windows XP with SP2 and above
#       define WINVER 0x0502
#       define _WIN32_WINNT 0x0502
#     endif // MU_ARCH_64BIT
#   endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#   define MU_PLATFORM_WINDOWS _WIN32_WINNT
# else
#   undef  MU_PLATFORM_WINRT
#   define MU_PLATFORM_WINRT 1
# endif
#elif defined(__VCCOREVER__)
// RaspberryPi compiler defines __linux__
# undef  MU_PLATFORM_RPI
# define MU_PLATFORM_RPI 1
#elif defined(__native_client__)
// NaCl compiler defines __linux__
# include <ppapi/c/pp_macros.h>
# undef  MU_PLATFORM_NACL
# define MU_PLATFORM_NACL PPAPI_RELEASE
#elif defined(__ANDROID__)
// Android compiler defines __linux__
# include <android/api-level.h>
# undef  MU_PLATFORM_ANDROID
# define MU_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__linux__)
# undef  MU_PLATFORM_LINUX
# define MU_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
# undef  MU_PLATFORM_IOS
# define MU_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
# undef  MU_PLATFORM_OSX
# define MU_PLATFORM_OSX 1
#elif defined(EMSCRIPTEN)
# undef  MU_PLATFORM_EMSCRIPTEN
# define MU_PLATFORM_EMSCRIPTEN 1
#elif defined(__FreeBSD__)
# undef  MU_PLATFORM_FREEBSD
# define MU_PLATFORM_FREEBSD 1
#else
# error "MU_PLATFORM_* is not defined!"
#endif //

#define MU_PLATFORM_POSIX (0 \
            || MU_PLATFORM_ANDROID \
            || MU_PLATFORM_EMSCRIPTEN \
            || MU_PLATFORM_FREEBSD \
            || MU_PLATFORM_IOS \
            || MU_PLATFORM_LINUX \
            || MU_PLATFORM_NACL \
            || MU_PLATFORM_OSX \
            || MU_PLATFORM_RPI \
            )

#if MU_COMPILER_GCC
# define MU_COMPILER_NAME "GCC " \
        MU_STRINGIZE(__GNUC__) "." \
        MU_STRINGIZE(__GNUC_MINOR__) "." \
        MU_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif MU_COMPILER_CLANG
# define MU_COMPILER_NAME "Clang " \
        MU_STRINGIZE(__clang_major__) "." \
        MU_STRINGIZE(__clang_minor__) "." \
        MU_STRINGIZE(__clang_patchlevel__)
#elif MU_COMPILER_MSVC
# if MU_COMPILER_MSVC >= 1900
#   define MU_COMPILER_NAME "MSVC 14.0"
# elif MU_COMPILER_MSVC >= 1800
#   define MU_COMPILER_NAME "MSVC 12.0"
# elif MU_COMPILER_MSVC >= 1700
#   define MU_COMPILER_NAME "MSVC 11.0"
# elif MU_COMPILER_MSVC >= 1600
#   define MU_COMPILER_NAME "MSVC 10.0"
# elif MU_COMPILER_MSVC >= 1500
#   define MU_COMPILER_NAME "MSVC 9.0"
# else
#   define MU_COMPILER_NAME "MSVC"
# endif //
#endif // MU_COMPILER_

#if MU_PLATFORM_ANDROID
# define MU_PLATFORM_NAME "Android " \
        MU_STRINGIZE(MU_PLATFORM_ANDROID)
#elif MU_PLATFORM_EMSCRIPTEN
# define MU_PLATFORM_NAME "asm.js " \
        MU_STRINGIZE(__EMSCRIPTEN_major__) "." \
        MU_STRINGIZE(__EMSCRIPTEN_minor__) "." \
        MU_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif MU_PLATFORM_FREEBSD
# define MU_PLATFORM_NAME "FreeBSD"
#elif MU_PLATFORM_IOS
# define MU_PLATFORM_NAME "iOS"
#elif MU_PLATFORM_LINUX
# define MU_PLATFORM_NAME "Linux"
#elif MU_PLATFORM_NACL
# define MU_PLATFORM_NAME "NaCl " \
        MU_STRINGIZE(MU_PLATFORM_NACL)
#elif MU_PLATFORM_OSX
# define MU_PLATFORM_NAME "OSX"
#elif MU_PLATFORM_PS4
# define MU_PLATFORM_NAME "PlayStation 4"
#elif MU_PLATFORM_QNX
# define MU_PLATFORM_NAME "QNX"
#elif MU_PLATFORM_RPI
# define MU_PLATFORM_NAME "RaspberryPi"
#elif MU_PLATFORM_WINDOWS
# define MU_PLATFORM_NAME "Windows"
#elif MU_PLATFORM_WINRT
# define MU_PLATFORM_NAME "WinRT"
#elif MU_PLATFORM_XBOX360
# define MU_PLATFORM_NAME "Xbox 360"
#elif MU_PLATFORM_XBOXONE
# define MU_PLATFORM_NAME "Xbox One"
#endif // MU_PLATFORM_

#if MU_CPU_ARM
# define MU_CPU_NAME "ARM"
#elif MU_CPU_MIPS
# define MU_CPU_NAME "MIPS"
#elif MU_CPU_PPC
# define MU_CPU_NAME "PowerPC"
#elif MU_CPU_JIT
# define MU_CPU_NAME "JIT-VM"
#elif MU_CPU_X86
# define MU_CPU_NAME "x86"
#endif // MU_CPU_

#if MU_ARCH_32BIT
# define MU_ARCH_NAME "32-bit"
#elif MU_ARCH_64BIT
# define MU_ARCH_NAME "64-bit"
#endif // MU_ARCH_

#endif