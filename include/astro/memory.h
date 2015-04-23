/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_MEMORY
#define ASTRO_MEMORY

#include "astro.h"
#include <string.h>
#include <functional>

namespace astro
{
  struct allocator
  {
    typedef std::function<void*(uintptr)> alloc_fun_t;
    typedef std::function<void(uint8*)> dealloc_fun_t;
    typedef std::function<void()> dispose_fun_t;

    alloc_fun_t allocate;
    dealloc_fun_t deallocate;
    dispose_fun_t dispose;

    // TODO: Default allocator uses malloc. Might want to change that...
    allocator() : allocate(::malloc), deallocate(::free), dispose([]{}) { }
    allocator(std::nullptr_t) : allocate(::malloc), deallocate(::free), dispose([]{}) { }
    allocator(void*) = delete;

    allocator(
      alloc_fun_t alloc, dealloc_fun_t dealloc,
      dispose_fun_t dispose = []{})
      : allocate(alloc)
      , deallocate(dealloc)
      , dispose(dispose)
    {
    }

    allocator(allocator const& rv)
    {
      *this = rv;
    }

    allocator& operator=(allocator const& rv)
    {
      this->allocate = rv.allocate;
      this->deallocate = rv.deallocate;
      this->dispose = rv.dispose;

      return *this;
    }

    allocator(allocator&& rv)
    {
      *this = std::move(rv);
    };

    allocator& operator=(allocator&& rv)
    {
      this->allocate = std::move(rv.allocate);
      this->deallocate = std::move(rv.deallocate);
      this->dispose = std::move(rv.dispose);

      return *this;
    }

    // allow for scope-based cleanup.
    ~allocator()
    {
      this->dispose();
    }
  };

  struct memory_pool
  {
    uintptr size;
    uintptr used;
    uint8* base;
    memory_pool* parent;
    void (*on_dispose)();
  };

  inline void
  initialize_memory_pool(memory_pool *pool, uintptr size, uint8 *base)
  {
    pool->size = size;
    pool->base = base;
    pool->used = 0;
  }

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

  inline memory_pool*
  push_pool(memory_pool* pool, uintptr size)
  {
    uintptr size_left = pool->size - pool->used;
    uintptr header_size = sizeof(memory_pool);
    astro_assert(size_left > header_size + size);

    uint8* base = (uint8*) push_size(pool, size + header_size);
    memory_pool* result = (memory_pool*) base;
    *result = {};
    result->size = size;
    result->base = base + header_size;
    result->parent = pool;

    return result;
  }

  inline memory_pool*
  push_pool(memory_pool* pool)
  {
    uintptr size_left = pool->size - pool->used;
    const uintptr header_size = sizeof(memory_pool);
    return push_pool(pool, size_left - header_size);
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
    const uintptr header_size = sizeof(memory_pool);
    astro_assert(pool->parent->base + (pool->parent->used - pool->size - header_size) == pool->base);

    pop_size(pool->parent, pool->size + header_size);
  }

  inline allocator memory_pool_allocator(memory_pool* pool, uintptr size)
  {
    memory_pool* child = push_pool(pool, size);
    return allocator
    (
      [child](uintptr size){ return push_size(child, size); },
      [child](uint8* ptr) { /* TODO: Cleanup. :) */ }
    );
  }
}

#endif
