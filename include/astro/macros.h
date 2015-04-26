/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_MACROS
#define ASTRO_MACROS

#include "astro.h"

#if !ASTRO_PLATFORM_WINDOWS
# include <signal.h>
#endif

#if ASTRO_COMPILER_MSVC
// Workaround MSVS bug...
# define ASTRO_VA_ARGS_PASS(...) ASTRO_VA_ARGS_PASS_1_ __VA_ARGS__ ASTRO_VA_ARGS_PASS_2_
# define ASTRO_VA_ARGS_PASS_1_ (
# define ASTRO_VA_ARGS_PASS_2_ )
#else
# define ASTRO_VA_ARGS_PASS(...) (__VA_ARGS__)
#endif // ASTRO_COMPILER_MSVC

#define ASTRO_VA_ARGS_COUNT(...) ASTRO_VA_ARGS_COUNT_ ASTRO_VA_ARGS_PASS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define ASTRO_VA_ARGS_COUNT_(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12, _a13, _a14, _a15, _a16, _last, ...) _last

///
#define ASTRO_MACRO_DISPATCHER(_func, ...) ASTRO_MACRO_DISPATCHER_1_(_func, ASTRO_VA_ARGS_COUNT(__VA_ARGS__) )
#define ASTRO_MACRO_DISPATCHER_1_(_func, _argCount) ASTRO_MACRO_DISPATCHER_2_(_func, _argCount)
#define ASTRO_MACRO_DISPATCHER_2_(_func, _argCount) ASTRO_CONCATENATE(_func, _argCount)

///
#define ASTRO_MAKEFOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

///
#define ASTRO_STRINGIZE(_x) ASTRO_STRINGIZE_(_x)
#define ASTRO_STRINGIZE_(_x) #_x

///
#define ASTRO_CONCATENATE(_x, _y) ASTRO_CONCATENATE_(_x, _y)
#define ASTRO_CONCATENATE_(_x, _y) _x ## _y

///
#define ASTRO_FILE_LINE_LITERAL "" __FILE__ "(" ASTRO_STRINGIZE(__LINE__) "): "

///
#define ASTRO_ALIGN_MASK(_value, _mask) ( ( (_value)+(_mask) ) & ( (~0)&(~(_mask) ) ) )
#define ASTRO_ALIGN_16(_value) ASTRO_ALIGN_MASK(_value, 0xf)
#define ASTRO_ALIGN_256(_value) ASTRO_ALIGN_MASK(_value, 0xff)
#define ASTRO_ALIGN_4096(_value) ASTRO_ALIGN_MASK(_value, 0xfff)

#define ASTRO_ALIGNOF(_type) __alignof(_type)

#if ASTRO_COMPILER_GCC || ASTRO_COMPILER_CLANG
# define ASTRO_ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
# define ASTRO_ALLOW_UNUSED __attribute__( (unused) )
# define ASTRO_FORCE_INLINE __extension__ static __inline __attribute__( (__always_inline__) )
# define ASTRO_FUNCTION __PRETTY_FUNCTION__
# define ASTRO_NO_INLINE __attribute__( (noinline) )
# define ASTRO_NO_RETURN __attribute__( (noreturn) )
# define ASTRO_NO_VTABLE
# define ASTRO_OVERRIDE
# define ASTRO_PRINTF_ARGS(_format, _args) __attribute__ ( (format(__printf__, _format, _args) ) )
# if ASTRO_COMPILER_CLANG && (ASTRO_PLATFORM_OSX || ASTRO_PLATFORM_IOS)
#   define ASTRO_THREAD /* not supported right now */
# else
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ <= 2)
#     define ASTRO_THREAD /* not supported right now */
#   else
#     define ASTRO_THREAD __thread
#   endif // __GNUC__ <= 4.2
# endif // ASTRO_COMPILER_CLANG
# define ASTRO_ATTRIBUTE(_x) __attribute__( (_x) )
# if ASTRO_COMPILER_MSVC_COMPATIBLE
#   define __stdcall
# endif // ASTRO_COMPILER_MSVC_COMPATIBLE
#elif ASTRO_COMPILER_MSVC
# define ASTRO_ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
# define ASTRO_ALLOW_UNUSED
# define ASTRO_FORCE_INLINE __forceinline
# define ASTRO_FUNCTION __FUNCTION__
# define ASTRO_NO_INLINE __declspec(noinline)
# define ASTRO_NO_RETURN
# define ASTRO_NO_VTABLE __declspec(novtable)
# define ASTRO_OVERRIDE override
# define ASTRO_PRINTF_ARGS(_format, _args)
# define ASTRO_THREAD __declspec(thread)
# define ASTRO_ATTRIBUTE(_x)
#else
# error "Unknown ASTRO_COMPILER_?"
#endif

#if defined(__has_extension)
# define ASTRO_CLANG_HAS_EXTENSION(_x) __has_extension(_x)
#else
# define ASTRO_CLANG_HAS_EXTENSION(_x) 0
#endif // defined(__has_extension)

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_trap)
# define ASTRO_DEBUG_BREAK __builtin_trap()
#elif ASTRO_PLATFORM_WINDOWS
# define ASTRO_DEBUG_BREAK __debugbreak()
#else
# define ASTRO_DEBUG_BREAK raise(SIGTRAP)
#endif

#if ASTRO_ASSERTIONS_ENABLED
# define astro_assert(expression)                                                    \
  if (!(expression))                                                           \
  {                                                                            \
    ASTRO_DEBUG_BREAK;                                                            \
  }
#else
# define astro_assert(expression)
#endif

// #define ASTRO_STATIC_ASSERT(_condition, ...) static_assert(_condition, "" __VA_ARGS__)
#define ASTRO_STATIC_ASSERT(_condition, ...) typedef char ASTRO_CONCATENATE(ASTRO_STATIC_ASSERT_, __LINE__)[1][(_condition)] ASTRO_ATTRIBUTE(unused)

///
#define ASTRO_ALIGN_DECL_16(_decl) ASTRO_ALIGN_DECL(16, _decl)
#define ASTRO_ALIGN_DECL_256(_decl) ASTRO_ALIGN_DECL(256, _decl)
#define ASTRO_ALIGN_DECL_CACHE_LINE(_decl) ASTRO_ALIGN_DECL(ASTRO_CACHE_LINE_SIZE, _decl)

///
#define ASTRO_MACRO_BLOCK_BEGIN for(;;) {
#define ASTRO_MACRO_BLOCK_END break; }
#define ASTRO_NOOP(...) ASTRO_MACRO_BLOCK_BEGIN ASTRO_MACRO_BLOCK_END

///
#define ASTRO_UNUSED_1(_a1) ASTRO_MACRO_BLOCK_BEGIN (void)(true ? (void)0 : ( (void)(_a1) ) ); ASTRO_MACRO_BLOCK_END
#define ASTRO_UNUSED_2(_a1, _a2) ASTRO_UNUSED_1(_a1); ASTRO_UNUSED_1(_a2)
#define ASTRO_UNUSED_3(_a1, _a2, _a3) ASTRO_UNUSED_2(_a1, _a2); ASTRO_UNUSED_1(_a3)
#define ASTRO_UNUSED_4(_a1, _a2, _a3, _a4) ASTRO_UNUSED_3(_a1, _a2, _a3); ASTRO_UNUSED_1(_a4)
#define ASTRO_UNUSED_5(_a1, _a2, _a3, _a4, _a5) ASTRO_UNUSED_4(_a1, _a2, _a3, _a4); ASTRO_UNUSED_1(_a5)
#define ASTRO_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6) ASTRO_UNUSED_5(_a1, _a2, _a3, _a4, _a5); ASTRO_UNUSED_1(_a6)
#define ASTRO_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7) ASTRO_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6); ASTRO_UNUSED_1(_a7)
#define ASTRO_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) ASTRO_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7); ASTRO_UNUSED_1(_a8)
#define ASTRO_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) ASTRO_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8); ASTRO_UNUSED_1(_a9)
#define ASTRO_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10) ASTRO_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9); ASTRO_UNUSED_1(_a10)
#define ASTRO_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11) ASTRO_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10); ASTRO_UNUSED_1(_a11)
#define ASTRO_UNUSED_12(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12) ASTRO_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11); ASTRO_UNUSED_1(_a12)

#if ASTRO_COMPILER_MSVC
// Workaround MSVS bug...
# define ASTRO_UNUSED(...) ASTRO_MACRO_DISPATCHER(ASTRO_UNUSED_, __VA_ARGS__) ASTRO_VA_ARGS_PASS(__VA_ARGS__)
#else
# define ASTRO_UNUSED(...) ASTRO_MACRO_DISPATCHER(ASTRO_UNUSED_, __VA_ARGS__)(__VA_ARGS__)
#endif // ASTRO_COMPILER_MSVC

///
#if ASTRO_COMPILER_CLANG
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_CLANG()      _Pragma("clang diagnostic push")
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_CLANG()       _Pragma("clang diagnostic pop")
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x) _Pragma(ASTRO_STRINGIZE(clang diagnostic ignored _x) )
#else
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_CLANG()
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_CLANG()
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x)
#endif // ASTRO_COMPILER_CLANG

#if ASTRO_COMPILER_GCC && ASTRO_COMPILER_GCC >= 40600
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_GCC()        _Pragma("GCC diagnostic push")
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_GCC()         _Pragma("GCC diagnostic pop")
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)   _Pragma(ASTRO_STRINGIZE(GCC diagnostic ignored _x) )
#else
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_GCC()
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_GCC()
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)
#endif // ASTRO_COMPILER_GCC

#if ASTRO_COMPILER_MSVC
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_MSVC()      __pragma(warning(push) )
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_MSVC()       __pragma(warning(pop) )
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x) __pragma(warning(disable:_x) )
#else
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH_MSVC()
# define ASTRO_PRAGMA_DIAGNOSTIC_POP_MSVC()
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x)
#endif // ASTRO_COMPILER_CLANG

#if ASTRO_COMPILER_CLANG
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH              ASTRO_PRAGMA_DIAGNOSTIC_PUSH_CLANG
# define ASTRO_PRAGMA_DIAGNOSTIC_POP               ASTRO_PRAGMA_DIAGNOSTIC_POP_CLANG
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG
#elif ASTRO_COMPILER_GCC
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH              ASTRO_PRAGMA_DIAGNOSTIC_PUSH_GCC
# define ASTRO_PRAGMA_DIAGNOSTIC_POP               ASTRO_PRAGMA_DIAGNOSTIC_POP_GCC
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_GCC
#elif ASTRO_COMPILER_MSVC
# define ASTRO_PRAGMA_DIAGNOSTIC_PUSH              ASTRO_PRAGMA_DIAGNOSTIC_PUSH_MSVC
# define ASTRO_PRAGMA_DIAGNOSTIC_POP               ASTRO_PRAGMA_DIAGNOSTIC_POP_MSVC
# define ASTRO_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC(_x)
#endif // ASTRO_COMPILER_

///
#if ASTRO_COMPILER_GCC && defined(__is_pod)
# define ASTRO_TYPE_IS_POD(t) __is_pod(t)
#elif ASTRO_COMPILER_MSVC
# define ASTRO_TYPE_IS_POD(t) (!__is_class(t) || __is_pod(t))
#else
# define ASTRO_TYPE_IS_POD(t) false
#endif
///
#define ASTRO_CLASS_NO_DEFAULT_CTOR(_class) \
      private: _class()

#define ASTRO_CLASS_NO_COPY(_class) \
      private: _class(const _class& _rhs)

#define ASTRO_CLASS_NO_ASSIGNMENT(_class) \
      private: _class& operator=(const _class& _rhs)

#define ASTRO_CLASS_ALLOCATOR(_class) \
      public: void* operator new(size_t _size); \
      public: void  operator delete(void* _ptr); \
      public: void* operator new[](size_t _size); \
      public: void  operator delete[](void* _ptr)

#define ASTRO_CLASS_1(_class, _a1) ASTRO_CONCATENATE(ASTRO_CLASS_, _a1)(_class)
#define ASTRO_CLASS_2(_class, _a1, _a2) ASTRO_CLASS_1(_class, _a1); ASTRO_CLASS_1(_class, _a2)
#define ASTRO_CLASS_3(_class, _a1, _a2, _a3) ASTRO_CLASS_2(_class, _a1, _a2); ASTRO_CLASS_1(_class, _a3)
#define ASTRO_CLASS_4(_class, _a1, _a2, _a3, _a4) ASTRO_CLASS_3(_class, _a1, _a2, _a3); ASTRO_CLASS_1(_class, _a4)

#if ASTRO_COMPILER_MSVC
# define ASTRO_CLASS(_class, ...) ASTRO_MACRO_DISPATCHER(ASTRO_CLASS_, __VA_ARGS__) ASTRO_VA_ARGS_PASS(_class, __VA_ARGS__)
#else
# define ASTRO_CLASS(_class, ...) ASTRO_MACRO_DISPATCHER(ASTRO_CLASS_, __VA_ARGS__)(_class, __VA_ARGS__)
#endif // ASTRO_COMPILER_MSVC

#ifndef ASTRO_CHECK
# define ASTRO_CHECK(_condition, ...) ASTRO_NOOP()
#endif // ASTRO_CHECK

#ifndef ASTRO_TRACE
# define ASTRO_TRACE(...) ASTRO_NOOP()
#endif // ASTRO_TRACE

#ifndef ASTRO_WARN
# define ASTRO_WARN(_condition, ...) ASTRO_NOOP()
#endif // ASTRO_CHECK

#define ASTRO_KB(value) ((value)*1024LL)
#define ASTRO_MB(value) (astro_kilobytes(value) * 1024LL)
#define ASTRO_GB(value) (astro_megabytes(value) * 1024LL)
#define ASTRO_TB(value) (astro_gigabytes(value) * 1024LL)

#endif
