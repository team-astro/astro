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
  struct file
  {
    static bool exists(const char* path);

    template <typename Allocator = allocator<char>>
    static const char* read_all_text(const char* path, Allocator alloc = Allocator());
  };
}}


#ifdef ASTRO_IMPLEMENTATION
#if ASTRO_PLATFORM_POSIX
#include "posix/file.inl"
#elif ASTRO_PLATFORM_WIN32
#include "win32/file.inl"
#else
#error "astro::io::file not implemented."
#endif
#endif

#endif
