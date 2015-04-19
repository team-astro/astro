/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

#ifndef ASTRO_MEMORY
#define ASTRO_MEMORY

#include "astro.h"
#include <string.h>

namespace astro
{
  struct memory_pool
  {
    uintptr size;
    uintptr used;
    uint8* base;
    memory_pool* parent;
    void (*on_dispose)();
  };

  void initialize_memory_pool(memory_pool *pool, uintptr size, uint8 *base);

  // TODO(matt): Throughly test this. This could be a really bad idea...
  template <typename t>
  inline t *
  convert_ptr_to_offset(memory_pool *pool, t * address)
  {
    if (!address) return address;

    uint8* a = (uint8*) address;
    intptr d = a - pool->base;
    return (t *) d;
  }

  template <typename t>
  inline t *
  convert_offset_to_ptr(memory_pool *pool, t * address)
  {
    if (!address) return address;

    intptr a = (intptr) address;
    uint8* ptr = pool->base + a;
    return (t *) ptr;
  }

  inline void*
  push_size(memory_pool *pool, uintptr size)
  {
    astro_assert(pool);
    astro_assert((pool->used + size) <= pool->size);

    void* result = pool->base + pool->used;
    pool->used += size;

    return result;
  }

  inline void
  pop_size(memory_pool* pool, uintptr size)
  {
    astro_assert(pool);
    astro_assert(pool->used >= size);

    pool->used -= size;
  }

  template <typename t>
  inline t*
  push_struct(memory_pool* pool)
  {
    return (t *) push_size(pool, sizeof(t));
  }

  template <typename t>
  inline t*
  push_array(memory_pool* pool, uintptr count)
  {
    return (t *) push_size(pool, sizeof(t) * count);
  }

  inline void
  push_pool(memory_pool* pool, memory_pool* parent, uintptr size)
  {
    astro_assert(pool);

    *pool = {};
    pool->base = (uint8*)push_size(parent, size);
    pool->size = size;
    pool->parent = parent;
  }

  inline void
  push_pool(memory_pool* pool, memory_pool* parent)
  {
    push_pool(pool, parent, parent->size - parent->used);
  }

  inline char*
  push_string(memory_pool *pool, const char* str)
  {
    auto len = strlen(str);
    char* dest = (char*) push_size(pool, len + 1);
    return strncpy(dest, str, len);
  }

  template <typename t>
  inline t*
  push_list(memory_pool* pool, t** list)
  {
    t* temp = *list;
    t* head = push_struct<t>(pool);
    head->next = temp;
    *list = head;

    return head;
  }

  inline void
  pop_pool(memory_pool* pool)
  {
    astro_assert(pool);
    astro_assert(pool->parent);

    // Ensure the parent pool has not allocated memory on the other side of this one.
    astro_assert(pool->parent->base + (pool->parent->used - pool->size) == pool->base);

    pop_size(pool->parent, pool->size);
  }

#if defined(ASTRO_IMPLEMENTATION)
void
initialize_memory_pool(memory_pool *pool, uintptr size, uint8 *base)
{
  pool->size = size;
  pool->base = base;
  pool->used = 0;
}
#endif
}

#endif
