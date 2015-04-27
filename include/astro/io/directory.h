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
  }
}}

#if ASTRO_PLATFORM_POSIX
#include "posix/directory.inl"
#else
#error "astro::io::directory not implemented."
#endif

#endif