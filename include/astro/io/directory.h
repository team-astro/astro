/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_IO_FILE_H
#define ASTRO_IO_FILE_H

#include <astro/astro.h>
#include <astro/string.h>
#include <stdarg.h>

namespace astro { namespace io
{
  struct directory
  {
    static bool exists(const char* path);
  };
}}

#ifdef ASTRO_IMPLEMENTATION
#if ASTRO_PLATFORM_POSIX
#include "posix/directory.inl"
#elif ASTRO_PLATFORM_WIN32
#include "win32/directory.inl"
#else
#error "astro::io::directory not implemented."
#endif
#endif
#endif
