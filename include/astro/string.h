/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef STRING_H
#define STRING_H

#include "astro.h"
#include "memory.h"
#include <cstring>
#include <cstdarg>

namespace astro
{
  inline char* strndup(const char* str, uintptr n, allocator* allocator = default_allocator)
  {
    astro_assert(str);
    astro_assert(strlen(str) >= n);

    char* result = nullptr;
    result = (char*) ASTRO_ALLOC(allocator, n + 1);
    strncpy(result, str, n);
    result[n] = '\0';

    return result;
  }

  inline char* strdup(const char* str, allocator* allocator = default_allocator)
  {
    return strndup(str, strlen(str), allocator);
  }

  // Allocate a new string with the correct length needed to
  // print the provided arguments.
  inline const char* saprintf(const char* fmt, allocator* allocator, ...)
  {
    va_list lst;
    va_start(lst, allocator);
    va_list lst_copy;
    va_copy(lst_copy, lst);

    int size = vsnprintf(nullptr, 0, fmt, lst) + 1;
    va_end(lst);

    char* result = (char*) ASTRO_ALLOC(allocator, size);
    vsnprintf(result, size, fmt, lst_copy);
    va_end(lst_copy);

    return result;
  }

  /*
   * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
   *
   * Permission to use, copy, modify, and distribute this software for any
   * purpose with or without fee is hereby granted, provided that the above
   * copyright notice and this permission notice appear in all copies.
   *
   * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
   */
  /// Copy src to string dst of size siz.  At most siz-1 characters
  /// will be copied.  Always NUL terminates (unless siz == 0).
  /// Returns strlen(src); if retval >= siz, truncation occurred.
  inline size_t strlcpy(char* dst, const char* src, size_t sz)
  {
    char* dd = dst;
    const char* ss = src;
    size_t nn = sz;

    /* Copy as many bytes as will fit */
    if (nn != 0)
    {
      while (--nn != 0)
      {
        if ( (*dd++ = *ss++) == '\0')
        {
          break;
        }
      }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (nn == 0)
    {
      if (sz != 0)
      {
        *dd = '\0';  /* NUL-terminate dst */
      }

      while (*ss++)
      {
      }
    }

    return(ss - src - 1); /* count does not include NUL */
  }

  /// Appends src to string dst of size siz (unlike strncat, siz is the
  /// full size of dst, not space left).  At most siz-1 characters
  /// will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
  /// Returns strlen(src) + MIN(siz, strlen(initial dst)).
  /// If retval >= siz, truncation occurred.
  inline size_t strlcat(char* dst, const char* src, size_t sz)
  {
    char* dd = dst;
    const char *s = src;
    size_t nn = sz;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while (nn-- != 0 && *dd != '\0')
    {
      dd++;
    }

    dlen = dd - dst;
    nn = sz - dlen;

    if (nn == 0)
    {
      return(dlen + strlen(s));
    }

    while (*s != '\0')
    {
      if (nn != 1)
      {
        *dd++ = *s;
        nn--;
      }
      s++;
    }
    *dd = '\0';

    return(dlen + (s - src)); /* count does not include NUL */
  }

  inline size_t
  append_format(char* dst, size_t dst_len, const char* fmt, ...)
  {
    char* dd = dst;
    size_t dlen;
    size_t len;

    while (*dd != '\0') ++dd;

    dlen = dd - dst;
    len = dst_len - dlen;

    va_list lst;
    va_start(lst, fmt);
    va_list lst_copy;
    va_copy(lst_copy, lst);

    int size_needed = vsnprintf(nullptr, 0, fmt, lst) + 1;
    va_end(lst);

    if (len < size_needed)
      return size_needed;

    size_t result = vsnprintf(dd, len, fmt, lst_copy);
    va_end(lst_copy);

    return result;
  }
}

#endif
