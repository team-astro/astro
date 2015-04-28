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
  template <typename Allocator = allocator<char>>
  inline char* strdup(const char* str, Allocator allocator = Allocator())
  {
    if (str == nullptr) return nullptr;

    char* result = nullptr;
    size_t len = strlen(str);
    result = (char*) allocator.allocate(len + 1);
    strncpy(result, str, len);
    result[len] = '\0';

    return result;
  }

  // Allocate a new string with the correct length needed to
  // print the provided arguments.
  template <typename Allocator = allocator<char>>
  inline const char* saprintf(const char* fmt, Allocator allocator, ...)
  {
    va_list lst;
    va_start(lst, allocator);
    va_list lst_copy;
    va_copy(lst_copy, lst);

    int size = vsnprintf(nullptr, 0, fmt, lst) + 1;
    va_end(lst);

    char* result = allocator.allocate(size);
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

}

#endif
