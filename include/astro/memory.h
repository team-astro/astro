/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_MEMORY
#define ASTRO_MEMORY

#include "astro.h"
#include <memory>
#include <cstring>
#include <functional>

namespace astro
{
  struct memory_pool
  {
    uintptr size;
    uintptr used;
    uint8* base;
    void* last;
    memory_pool* parent;
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
    pool->last = result;

    return result;
  }

  inline void
  pop_size(memory_pool* pool, uintptr size)
  {
    astro_assert(pool);
    astro_assert(pool->used >= size);

    pool->used -= size;
  }

  inline bool
  pop_value(memory_pool* pool, void* value, uintptr size)
  {
    if (pool->last == value)
    {
      pop_size(pool, size);
      return true;
    }

    return false;
  }

  template <typename t>
  inline t*
  push_struct(memory_pool* pool)
  {
    return (t *) push_size(pool, sizeof(t));
  }

  template <typename t>
  inline bool32
  pop_struct(memory_pool* pool, t* value)
  {
    return pop_value(pool, (void*)value, sizeof(t));
  }

  template <typename t>
  inline t*
  push_array(memory_pool* pool, uintptr count)
  {
    return (t *) push_size(pool, sizeof(t) * count);
  }

  template <typename t>
  inline bool32
  pop_array(memory_pool* pool, t* value, uintptr count)
  {
    return pop_value(pool, (void*)value, sizeof(t) * count);
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

  inline bool
  pop_string(memory_pool* pool, const char* value)
  {
    auto len = strlen(value);
    return pop_array<const char>(pool, value, len + 1);
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


  template <typename T>
  class memory_pool_allocator
  {
    memory_pool* m_pool;

  public:
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;
    typedef const T *const_pointer;
    typedef const T &const_reference;
    typedef unsigned size_type;
    typedef unsigned difference_type;
    template <typename U>
    struct rebind
    {
      typedef memory_pool_allocator<U> other;
    };

    memory_pool_allocator(memory_pool* pool)
      : m_pool(pool)
    {
    }

    pointer allocate(unsigned n)
    {
      return reinterpret_cast<T *>(push_size(m_pool, sizeof(T) * n));
    }

    void deallocate(pointer p, unsigned n)
    {
      pop_value(m_pool, p, sizeof(T) * n);
    }

    void construct(pointer p, const_reference clone)
    {
      new (p) T(clone);
    }

    void destroy(pointer p)
    {
      p->~T();
    }

    pointer address(reference x) const
    {
      return &x;
    }

    const_pointer address(const_reference x) const
    {
      return &x;
    }

    bool operator==(const memory_pool_allocator &rhs)
    {
        return true;
    }

    bool operator!=(const memory_pool_allocator &rhs)
    {
      return !operator==(rhs);
    }
  };

  // TODO: Better default allocator.
  template <typename T> using allocator = std::allocator<T>;
}

#endif
