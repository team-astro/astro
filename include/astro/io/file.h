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

    template <typename Allocator = allocator<const char>>
    static const char* read_all_text(const char* path, Allocator alloc = Allocator());
  };
}}


#if ASTRO_PLATFORM_POSIX
#include "posix/file.inl"
#else
#error "astro::io::file not implemented."
#endif

#endif
