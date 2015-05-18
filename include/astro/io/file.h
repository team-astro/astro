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
    static const char* read_all_text(const char* path, allocator* alloc = default_allocator);
  };
}}

#endif
