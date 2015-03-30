/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

#ifndef MU_MACROS
#define MU_MACROS

#include "mu.h"

#if MU_COMPILER_MSVC
// Workaround MSVS bug...
# define MU_VA_ARGS_PASS(...) MU_VA_ARGS_PASS_1_ __VA_ARGS__ MU_VA_ARGS_PASS_2_
# define MU_VA_ARGS_PASS_1_ (
# define MU_VA_ARGS_PASS_2_ )
#else
# define MU_VA_ARGS_PASS(...) (__VA_ARGS__)
#endif // MU_COMPILER_MSVC

#define MU_VA_ARGS_COUNT(...) MU_VA_ARGS_COUNT_ MU_VA_ARGS_PASS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define MU_VA_ARGS_COUNT_(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12, _a13, _a14, _a15, _a16, _last, ...) _last

///
#define MU_MACRO_DISPATCHER(_func, ...) MU_MACRO_DISPATCHER_1_(_func, MU_VA_ARGS_COUNT(__VA_ARGS__) )
#define MU_MACRO_DISPATCHER_1_(_func, _argCount) MU_MACRO_DISPATCHER_2_(_func, _argCount)
#define MU_MACRO_DISPATCHER_2_(_func, _argCount) MU_CONCATENATE(_func, _argCount)

///
#define MU_MAKEFOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

///
#define MU_STRINGIZE(_x) MU_STRINGIZE_(_x)
#define MU_STRINGIZE_(_x) #_x

///
#define MU_CONCATENATE(_x, _y) MU_CONCATENATE_(_x, _y)
#define MU_CONCATENATE_(_x, _y) _x ## _y

///
#define MU_FILE_LINE_LITERAL "" __FILE__ "(" MU_STRINGIZE(__LINE__) "): "

///
#define MU_ALIGN_MASK(_value, _mask) ( ( (_value)+(_mask) ) & ( (~0)&(~(_mask) ) ) )
#define MU_ALIGN_16(_value) MU_ALIGN_MASK(_value, 0xf)
#define MU_ALIGN_256(_value) MU_ALIGN_MASK(_value, 0xff)
#define MU_ALIGN_4096(_value) MU_ALIGN_MASK(_value, 0xfff)

#define MU_ALIGNOF(_type) __alignof(_type)

#if MU_COMPILER_GCC || MU_COMPILER_CLANG
# define MU_ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
# define MU_ALLOW_UNUSED __attribute__( (unused) )
# define MU_FORCE_INLINE __extension__ static __inline __attribute__( (__always_inline__) )
# define MU_FUNCTION __PRETTY_FUNCTION__
# define MU_NO_INLINE __attribute__( (noinline) )
# define MU_NO_RETURN __attribute__( (noreturn) )
# define MU_NO_VTABLE
# define MU_OVERRIDE
# define MU_PRINTF_ARGS(_format, _args) __attribute__ ( (format(__printf__, _format, _args) ) )
# if MU_COMPILER_CLANG && (MU_PLATFORM_OSX || MU_PLATFORM_IOS)
#   define MU_THREAD /* not supported right now */
# else
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ <= 2)
#     define MU_THREAD /* not supported right now */  
#   else
#     define MU_THREAD __thread
#   endif // __GNUC__ <= 4.2
# endif // MU_COMPILER_CLANG
# define MU_ATTRIBUTE(_x) __attribute__( (_x) )
# if MU_COMPILER_MSVC_COMPATIBLE
#   define __stdcall
# endif // MU_COMPILER_MSVC_COMPATIBLE
#elif MU_COMPILER_MSVC
# define MU_ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
# define MU_ALLOW_UNUSED
# define MU_FORCE_INLINE __forceinline
# define MU_FUNCTION __FUNCTION__
# define MU_NO_INLINE __declspec(noinline)
# define MU_NO_RETURN
# define MU_NO_VTABLE __declspec(novtable)
# define MU_OVERRIDE override
# define MU_PRINTF_ARGS(_format, _args)
# define MU_THREAD __declspec(thread)
# define MU_ATTRIBUTE(_x)
#else
# error "Unknown MU_COMPILER_?"
#endif

#if defined(__has_extension)
# define MU_CLANG_HAS_EXTENSION(_x) __has_extension(_x)
#else
# define MU_CLANG_HAS_EXTENSION(_x) 0
#endif // defined(__has_extension)

// #define MU_STATIC_ASSERT(_condition, ...) static_assert(_condition, "" __VA_ARGS__)
#define MU_STATIC_ASSERT(_condition, ...) typedef char MU_CONCATENATE(MU_STATIC_ASSERT_, __LINE__)[1][(_condition)] MU_ATTRIBUTE(unused)

///
#define MU_ALIGN_DECL_16(_decl) MU_ALIGN_DECL(16, _decl)
#define MU_ALIGN_DECL_256(_decl) MU_ALIGN_DECL(256, _decl)
#define MU_ALIGN_DECL_CACHE_LINE(_decl) MU_ALIGN_DECL(MU_CACHE_LINE_SIZE, _decl)

///
#define MU_MACRO_BLOCK_BEGIN for(;;) {
#define MU_MACRO_BLOCK_END break; }
#define MU_NOOP(...) MU_MACRO_BLOCK_BEGIN MU_MACRO_BLOCK_END

///
#define MU_UNUSED_1(_a1) MU_MACRO_BLOCK_BEGIN (void)(true ? (void)0 : ( (void)(_a1) ) ); MU_MACRO_BLOCK_END
#define MU_UNUSED_2(_a1, _a2) MU_UNUSED_1(_a1); MU_UNUSED_1(_a2)
#define MU_UNUSED_3(_a1, _a2, _a3) MU_UNUSED_2(_a1, _a2); MU_UNUSED_1(_a3)
#define MU_UNUSED_4(_a1, _a2, _a3, _a4) MU_UNUSED_3(_a1, _a2, _a3); MU_UNUSED_1(_a4)
#define MU_UNUSED_5(_a1, _a2, _a3, _a4, _a5) MU_UNUSED_4(_a1, _a2, _a3, _a4); MU_UNUSED_1(_a5)
#define MU_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6) MU_UNUSED_5(_a1, _a2, _a3, _a4, _a5); MU_UNUSED_1(_a6)
#define MU_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7) MU_UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6); MU_UNUSED_1(_a7)
#define MU_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) MU_UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7); MU_UNUSED_1(_a8)
#define MU_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) MU_UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8); MU_UNUSED_1(_a9)
#define MU_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10) MU_UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9); MU_UNUSED_1(_a10)
#define MU_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11) MU_UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10); MU_UNUSED_1(_a11)
#define MU_UNUSED_12(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12) MU_UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11); MU_UNUSED_1(_a12)

#if MU_COMPILER_MSVC
// Workaround MSVS bug...
# define MU_UNUSED(...) MU_MACRO_DISPATCHER(MU_UNUSED_, __VA_ARGS__) MU_VA_ARGS_PASS(__VA_ARGS__)
#else
# define MU_UNUSED(...) MU_MACRO_DISPATCHER(MU_UNUSED_, __VA_ARGS__)(__VA_ARGS__)
#endif // MU_COMPILER_MSVC

///
#if MU_COMPILER_CLANG
# define MU_PRAGMA_DIAGNOSTIC_PUSH_CLANG()      _Pragma("clang diagnostic push")
# define MU_PRAGMA_DIAGNOSTIC_POP_CLANG()       _Pragma("clang diagnostic pop")
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x) _Pragma(MU_STRINGIZE(clang diagnostic ignored _x) )
#else
# define MU_PRAGMA_DIAGNOSTIC_PUSH_CLANG()
# define MU_PRAGMA_DIAGNOSTIC_POP_CLANG()
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x)
#endif // MU_COMPILER_CLANG

#if MU_COMPILER_GCC && MU_COMPILER_GCC >= 40600
# define MU_PRAGMA_DIAGNOSTIC_PUSH_GCC()        _Pragma("GCC diagnostic push")
# define MU_PRAGMA_DIAGNOSTIC_POP_GCC()         _Pragma("GCC diagnostic pop")
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)   _Pragma(MU_STRINGIZE(GCC diagnostic ignored _x) )
#else
# define MU_PRAGMA_DIAGNOSTIC_PUSH_GCC()
# define MU_PRAGMA_DIAGNOSTIC_POP_GCC()
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)
#endif // MU_COMPILER_GCC

#if MU_COMPILER_MSVC
# define MU_PRAGMA_DIAGNOSTIC_PUSH_MSVC()      __pragma(warning(push) )
# define MU_PRAGMA_DIAGNOSTIC_POP_MSVC()       __pragma(warning(pop) )
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x) __pragma(warning(disable:_x) )
#else
# define MU_PRAGMA_DIAGNOSTIC_PUSH_MSVC()
# define MU_PRAGMA_DIAGNOSTIC_POP_MSVC()
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x)
#endif // MU_COMPILER_CLANG

#if MU_COMPILER_CLANG
# define MU_PRAGMA_DIAGNOSTIC_PUSH              MU_PRAGMA_DIAGNOSTIC_PUSH_CLANG
# define MU_PRAGMA_DIAGNOSTIC_POP               MU_PRAGMA_DIAGNOSTIC_POP_CLANG
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG
#elif MU_COMPILER_GCC
# define MU_PRAGMA_DIAGNOSTIC_PUSH              MU_PRAGMA_DIAGNOSTIC_PUSH_GCC
# define MU_PRAGMA_DIAGNOSTIC_POP               MU_PRAGMA_DIAGNOSTIC_POP_GCC
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC MU_PRAGMA_DIAGNOSTIC_IGNORED_GCC
#elif MU_COMPILER_MSVC
# define MU_PRAGMA_DIAGNOSTIC_PUSH              MU_PRAGMA_DIAGNOSTIC_PUSH_MSVC
# define MU_PRAGMA_DIAGNOSTIC_POP               MU_PRAGMA_DIAGNOSTIC_POP_MSVC
# define MU_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC(_x)
#endif // MU_COMPILER_

///
#if MU_COMPILER_GCC && defined(__is_pod)
# define MU_TYPE_IS_POD(t) __is_pod(t)
#elif MU_COMPILER_MSVC
# define MU_TYPE_IS_POD(t) (!__is_class(t) || __is_pod(t))
#else
# define MU_TYPE_IS_POD(t) false
#endif 
///
#define MU_CLASS_NO_DEFAULT_CTOR(_class) \
      private: _class()

#define MU_CLASS_NO_COPY(_class) \
      private: _class(const _class& _rhs)

#define MU_CLASS_NO_ASSIGNMENT(_class) \
      private: _class& operator=(const _class& _rhs)

#define MU_CLASS_ALLOCATOR(_class) \
      public: void* operator new(size_t _size); \
      public: void  operator delete(void* _ptr); \
      public: void* operator new[](size_t _size); \
      public: void  operator delete[](void* _ptr)

#define MU_CLASS_1(_class, _a1) MU_CONCATENATE(MU_CLASS_, _a1)(_class)
#define MU_CLASS_2(_class, _a1, _a2) MU_CLASS_1(_class, _a1); MU_CLASS_1(_class, _a2)
#define MU_CLASS_3(_class, _a1, _a2, _a3) MU_CLASS_2(_class, _a1, _a2); MU_CLASS_1(_class, _a3)
#define MU_CLASS_4(_class, _a1, _a2, _a3, _a4) MU_CLASS_3(_class, _a1, _a2, _a3); MU_CLASS_1(_class, _a4)

#if MU_COMPILER_MSVC
# define MU_CLASS(_class, ...) MU_MACRO_DISPATCHER(MU_CLASS_, __VA_ARGS__) MU_VA_ARGS_PASS(_class, __VA_ARGS__)
#else
# define MU_CLASS(_class, ...) MU_MACRO_DISPATCHER(MU_CLASS_, __VA_ARGS__)(_class, __VA_ARGS__)
#endif // MU_COMPILER_MSVC

#ifndef MU_CHECK
# define MU_CHECK(_condition, ...) MU_NOOP()
#endif // MU_CHECK

#ifndef MU_TRACE
# define MU_TRACE(...) MU_NOOP()
#endif // MU_TRACE

#ifndef MU_WARN
# define MU_WARN(_condition, ...) MU_NOOP()
#endif // MU_CHECK

#endif