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
  struct memory_stack
  {
    uintptr size;
    uintptr used;
    uint8* base;
    void* last;
    memory_stack* parent;
  };

  inline void
  initialize_memory_stack(memory_stack * stack, uintptr size, uint8 *base)
  {
    stack->size = size;
    stack->base = base;
    stack->used = 0;
  }

  // TODO(matt): Throughly test this. This could be a really bad idea...
  template <typename t>
  inline t *
  convert_ptr_to_offset(memory_stack * stack, t * address)
  {
    if (!address) return address;

    uint8* a = (uint8*) address;
    intptr d = a - stack->base;
    return (t *) d;
  }

  template <typename t>
  inline t *
  convert_offset_to_ptr(memory_stack * stack, t * address)
  {
    if (!address) return address;

    intptr a = (intptr) address;
    uint8* ptr = stack->base + a;
    return (t *) ptr;
  }

  inline void*
  push_size(memory_stack * stack, uintptr size)
  {
    astro_assert(stack);
    astro_assert((stack->used + size) <= stack->size);

    void* result = stack->base + stack->used;
    stack->used += size;
    stack->last = result;

    return result;
  }

  inline void
  pop_size(memory_stack* stack, uintptr size)
  {
    astro_assert(stack);
    astro_assert(stack->used >= size);

    stack->used -= size;
  }

  inline bool
  pop_value(memory_stack* stack, void* value, uintptr size)
  {
    if (stack->last == value)
    {
      pop_size(stack, size);
      return true;
    }

    return false;
  }

  template <typename t>
  inline t*
  push_struct(memory_stack* stack)
  {
    return (t *) push_size(stack, sizeof(t));
  }

  template <typename t>
  inline bool32
  pop_struct(memory_stack* stack, t* value)
  {
    return pop_value(stack, (void*)value, sizeof(t));
  }

  template <typename t>
  inline t*
  push_array(memory_stack* stack, uintptr count)
  {
    return (t *) push_size(stack, sizeof(t) * count);
  }

  template <typename t>
  inline bool32
  pop_array(memory_stack* stack, t* value, uintptr count)
  {
    return pop_value(stack, (void*)value, sizeof(t) * count);
  }

  inline memory_stack*
  push_stack(memory_stack* stack, uintptr size)
  {
    uintptr size_left = stack->size - stack->used;
    uintptr header_size = sizeof(memory_stack);
    astro_assert(size_left > header_size + size);

    uint8* base = (uint8*) push_size(stack, size + header_size);
    memory_stack* result = (memory_stack*) base;
    *result = {};
    result->size = size;
    result->base = base + header_size;
    result->parent = stack;

    return result;
  }

  inline memory_stack*
  push_stack(memory_stack* stack)
  {
    uintptr size_left = stack->size - stack->used;
    const uintptr header_size = sizeof(memory_stack);
    return push_stack(stack, size_left - header_size);
  }

  inline char*
  push_string(memory_stack * stack, const char* str)
  {
    auto len = strlen(str);
    char* dest = (char*) push_size(stack, len + 1);
    return strncpy(dest, str, len);
  }

  inline bool
  pop_string(memory_stack* stack, const char* value)
  {
    auto len = strlen(value);
    return pop_array<const char>(stack, value, len + 1);
  }

  template <typename t>
  inline t*
  push_list(memory_stack* stack, t** list)
  {
    t* temp = *list;
    t* head = push_struct<t>(stack);
    head->next = temp;
    *list = head;

    return head;
  }

  inline void
  pop_pool(memory_stack* stack)
  {
    astro_assert(stack);
    astro_assert(stack->parent);

    // Ensure the parent stack has not allocated memory on the other side of this one.
    const uintptr header_size = sizeof(memory_stack);
    astro_assert(stack->parent->base + (stack->parent->used - stack->size - header_size) == stack->base);

    pop_size(stack->parent, stack->size + header_size);
  }


  template <typename T>
  class memory_stack_allocator
  {
    memory_stack* m_stack;

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
      typedef memory_stack_allocator<U> other;
    };

    memory_stack_allocator(memory_stack* stack)
      : m_stack(stack)
    {
    }

    pointer allocate(unsigned n)
    {
      return reinterpret_cast<T *>(push_size(m_stack, sizeof(T) * n));
    }

    void deallocate(pointer p, unsigned n)
    {
      pop_value(m_stack, p, sizeof(T) * n);
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

    bool operator==(const memory_stack_allocator &rhs)
    {
        return this->m_stack == rhs.m_stack;
    }

    bool operator!=(const memory_stack_allocator &rhs)
    {
      return !operator==(rhs);
    }
  };

  // TODO: Better default allocator.
  template <typename T> using allocator = std::allocator<T>;
}

#endif
