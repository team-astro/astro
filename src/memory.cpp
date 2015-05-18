/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/astro.h>
#include <astro/memory.h>

namespace astro
{
  static crt_allocator crt_alloc;

  // TODO: Add config for default allocator.
  allocator* default_allocator = &crt_alloc;
}
