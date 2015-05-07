/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_MEMORY
#define ASTRO_MEMORY

#include "astro.h"
#include <memory>
#include <cstring>
#include <functional>

#if ASTRO_CONFIG_ALLOCATOR_DEBUG
#  define ASTRO_ALLOC(_allocator, _size)                         astro::alloc(_allocator, _size, 0, __FILE__, __LINE__)
#  define ASTRO_REALLOC(_allocator, _ptr, _size)                 astro::realloc(_allocator, _ptr, _size, 0, __FILE__, __LINE__)
#  define ASTRO_FREE(_allocator, _ptr)                           astro::free(_allocator, _ptr, 0, __FILE__, __LINE__)
#  define ASTRO_ALIGNED_ALLOC(_allocator, _size, _align)         astro::alloc(_allocator, _size, _align, __FILE__, __LINE__)
#  define ASTRO_ALIGNED_REALLOC(_allocator, _ptr, _size, _align) astro::realloc(_allocator, _ptr, _size, _align, __FILE__, __LINE__)
#  define ASTRO_ALIGNED_FREE(_allocator, _ptr, _align)           astro::free(_allocator, _ptr, _align, __FILE__, __LINE__)
#  define ASTRO_NEW(_allocator, _type)                           ::new(ASTRO_ALLOC(_allocator, sizeof(_type) ) ) _type
#  define ASTRO_DELETE(_allocator, _ptr)                         astro::delete_object(_allocator, _ptr, 0, __FILE__, __LINE__)
#  define ASTRO_ALIGNED_NEW(_allocator, _type, _align)           ::new(ASTRO_ALIGNED_ALLOC(_allocator, sizeof(_type), _align) ) _type
#  define ASTRO_ALIGNED_DELETE(_allocator, _ptr, _align)         astro::delete_object(_allocator, _ptr, _align, __FILE__, __LINE__)
#else
#  define ASTRO_ALLOC(_allocator, _size)                         astro::alloc(_allocator, _size, 0)
#  define ASTRO_REALLOC(_allocator, _ptr, _size)                 astro::realloc(_allocator, _ptr, _size, 0)
#  define ASTRO_FREE(_allocator, _ptr)                           astro::free(_allocator, _ptr, 0)
#  define ASTRO_ALIGNED_ALLOC(_allocator, _size, _align)         astro::alloc(_allocator, _size, _align)
#  define ASTRO_ALIGNED_REALLOC(_allocator, _ptr, _size, _align) astro::realloc(_allocator, _ptr, _size, _align)
#  define ASTRO_ALIGNED_FREE(_allocator, _ptr, _align)           astro::free(_allocator, _ptr, _align)
#  define ASTRO_NEW(_allocator, _type)                           ::new(ASTRO_ALLOC(_allocator, sizeof(_type) ) ) _type
#  define ASTRO_DELETE(_allocator, _ptr)                         astro::delete_object(_allocator, _ptr, 0)
#  define ASTRO_ALIGNED_NEW(_allocator, _type, _align)           ::new(ASTRO_ALIGNED_ALLOC(_allocator, sizeof(_type), _align) ) _type
#  define ASTRO_ALIGNED_DELETE(_allocator, _ptr, _align)         astro::delete_object(_allocator, _ptr, _align)
#endif

#ifndef ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#  define ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif

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
  pop_value(memory_stack* stack, void* value)
  {
    if (stack->last == value)
    {
      uint8* head = stack->base + stack->used;
      uintptr size = (uintptr)(head - (uint8*)value);
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
    return pop_value(stack, (void*)value);
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
    return pop_value(stack, (void*)value);
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

  inline bool32
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

  struct ASTRO_NO_VTABLE allocator
  {
    virtual ~allocator() = 0;
    virtual void* alloc(uintptr size, uintptr align, const char* file, uintptr line) = 0;
    virtual void free(void* p, uintptr align, const char* file, uintptr line) = 0;
  };

  inline allocator::~allocator()
  {
  }

  struct ASTRO_NO_VTABLE reallocator : public allocator
  {
    virtual void* realloc(void* p, uintptr size, uintptr align, const char* file, uintptr line) = 0;
  };

  inline void*
  align_pointer(void* ptr, uintptr extra, uintptr align = ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
  {
    union { void* ptr; uintptr addr; } un;
    un.ptr = ptr;
    uintptr unaligned = un.addr + extra;
    uintptr mask = align - 1;
    uintptr aligned = ASTRO_ALIGN_MASK(unaligned, mask);
    un.addr = aligned;
    return ptr;
  }

  inline bool32
  is_pointer_aligned(void* ptr, uintptr align = ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT)
  {
    union { void* ptr; uintptr addr; } un;
    un.ptr = ptr;
    return 0 == (un.addr & (align - 1));
  }

  inline void*
  alloc(allocator* allocator, uintptr size, uintptr align = 0, const char* file = nullptr, uintptr line = 0)
  {
    return allocator->alloc(size, align, file, line);
  }

  inline void
  free(allocator* allocator, void* ptr, uintptr align = 0, const char* file = nullptr, uintptr line = 0)
  {
    return allocator->free(ptr, align, file, line);
  }

  inline void*
  realloc(reallocator* allocator, void* ptr, uintptr size, uintptr align = 0, const char* file = nullptr, uintptr line = 0)
  {
    return allocator->realloc(ptr, size, align, file, line);
  }

  static inline void*
  aligned_alloc(allocator* allocator, uintptr size, uintptr align, const char* file = nullptr, uintptr line = 0)
  {
    uintptr total = size + align;
    uint8* ptr = (uint8*) alloc(allocator, total, 0, file, line);
    uint8* aligned = (uint8*) align_pointer(ptr, sizeof(uint32), align);
    uint32* header = (uint32*)aligned - 1;
    *header = uint32(aligned - ptr);
    return aligned;
  }

  static inline void
  aligned_free(allocator* allocator, void* ptr, uintptr /*align*/, const char* file = nullptr, uintptr line = 0)
  {
    uint8* aligned = (uint8*) ptr;
    uint32* header = (uint32*) aligned - 1;
    uint8* fptr = aligned - *header;
    free(allocator, fptr, 0, file, line);
  }

  static inline void*
  aligned_realloc(reallocator* allocator, void* ptr, uintptr size, uintptr align, const char* file = nullptr, uintptr line = 0)
  {
    if (ptr == nullptr)
    {
      return aligned_alloc(allocator, size, align, file, line);
    }

    uint8* aligned = (uint8*) ptr;
    uint32 offset = *((uint32*) aligned - 1);
    uint8* fptr = aligned - offset;
    uintptr total = size + align;
    ptr = (uint8*) realloc(allocator, ptr, total, 0, file, line);
    uint8* new_aligned = (uint8*)align_pointer(ptr, sizeof(uint32), align);

    if (new_aligned == aligned)
    {
      return aligned;
    }

    aligned = fptr + offset;
    ::memmove(new_aligned, aligned, size);
    uint32* header = (uint32*)new_aligned - 1;
    *header = uint32(new_aligned - fptr);
    return new_aligned;
  }

  template <typename T>
  inline void delete_object(allocator* allocator, T* obj, uintptr align = 0, const char* file = nullptr, uintptr line = 0)
  {
    if (obj != nullptr)
    {
      obj->~T();
      free(allocator, obj, align, file, line);
    }
  }

  struct crt_allocator : public reallocator
  {
    crt_allocator() {}
    virtual ~crt_allocator() {}

    virtual void* alloc(uintptr size, uintptr align, const char* file, uintptr line)
    {
      if (ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= align)
        return ::malloc(size);

#if ASTRO_COMPILER_MSVC
      ASTRO_UNUSED(file, line);
      return _aligned_malloc(size, align);
#else
      return astro::aligned_alloc(this, size, align, file, line);
#endif
    }

    virtual void free(void* p, uintptr align, const char* file, uintptr line)
    {
      if (ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= align)
      {
        ::free(p);
        return;
      }

#if ASTRO_COMPILER_MSVC
      ASTRO_UNUSED(file, line);
      return _aligned_free(p);
#else
      return astro::aligned_free(this, p, align, file, line);
#endif
    }

    virtual void* realloc(void* p, uintptr size, uintptr align, const char* file, uintptr line)
    {
      if (ASTRO_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= align)
        return ::realloc(p, size);

#if ASTRO_COMPILER_MSVC
      ASTRO_UNUSED(file, line);
      return _aligned_realloc(p, size, align);
#else
      return astro::aligned_realloc(this, p, size, align, file, line);
#endif
    }
  };

  class memory_stack_allocator : public allocator
  {
  public:
    memory_stack_allocator(memory_stack* stack)
      : m_stack(stack)
    {

    }

    virtual void* alloc(uintptr size, uintptr align, const char* file, uintptr line)
    {
      return push_size(m_stack, size);
    }

    virtual void free(void* p, uintptr align, const char* file, uintptr line)
    {
      pop_value(m_stack, p);
    }

  protected:
    memory_stack* m_stack;
  };

  class static_allocator : public memory_stack_allocator
  {
  public:
    static_allocator(uint8* buffer, uintptr buffer_size)
      : memory_stack_allocator(&stack)
    {
      initialize_memory_stack(&stack, buffer_size, buffer);
    }

  private:
    memory_stack stack;
  };

  // TODO: Add config for default allocator.
#ifdef ASTRO_IMPLEMENTATION
  static crt_allocator crt_alloc;
  static allocator* default_allocator = &crt_alloc;
#endif

  extern allocator* default_allocator;
}



#endif
