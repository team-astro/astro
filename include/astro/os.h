/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_OS
#define ASTRO_OS

#include <astro/astro.h>

#if ASTRO_PLATFORM_WIN32 || ASTRO_PLATFORM_WINRT
# define WIN32_LEAN_AND_MEAN 1
# include <windows.h>
#endif

#if ASTRO_PLATFORM_OSX
# define ASTRO_DL_EXT "dylib"
#elif ASTRO_PLATFORM_WIN32 || ASTRO_PLATFORM_WINRT
# define ASTRO_DL_EXT "dll"
#else
# define ASTRO_DL_EXT "so"
#endif

namespace astro
{
  inline void* dlopen(const char* path)
  {
#if ASTRO_PLATFORM_WIN32
    return (void*) ::LoadLibraryA(path);
#elif ASTRO_PLATFORM_NACL || ASTRO_PLATFORM_EMSCRIPTEN || ASTRO_PLATFORM_WINRT
    return nullptr;
#else
    return ::dlopen(path, RTLD_LOCAL | RTLD_LAZY);
#endif
  }

  inline void* dlclose(void* handle)
  {
#if ASTRO_PLATFORM_WIN32
    return (void*) ::FreeLibrary((HMODULE) handle);
#elif ASTRO_PLATFORM_NACL || ASTRO_PLATFORM_EMSCRIPTEN || ASTRO_PLATFORM_WINRT
    return nullptr;
#else
    return ::dlclose(handle);
#endif
  }

  inline void* dlsym(void* handle, const char* symbol)
  {
#if ASTRO_PLATFORM_WIN32
    return (void*) ::GetProcAddress((HMODULE)handle, symbol);
#elif ASTRO_PLATFORM_NACL || ASTRO_PLATFORM_EMSCRIPTEN || ASTRO_PLATFORM_WINRT
    return nullptr;
#else
    return ::dlsym(handle, symbol);
#endif
  }
}

#endif
