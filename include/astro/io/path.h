/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef IO_PATH_H
#define IO_PATH_H

#include <astro/astro.h>
#include <astro/string.h>
#include <stdarg.h>

namespace astro { namespace io { namespace path
{
  extern const char* volume_separator;
  extern const char* path_separator;
  extern const char* dir_separator;
  extern const char* alt_dir_separator;
  extern const char* invalid_filename_chars[];
  extern const char* invalid_path_chars[];

  inline bool
  is_path_rooted(const char* path)
  {
    if (!path)
      return false;

    size_t plen = strlen(path);
    if (plen == 0)
      return false;

    char c = path[0];
    return c == *dir_separator ||
          c == *alt_dir_separator ||
          (*dir_separator != *volume_separator && plen > 1 && path[1] == *volume_separator);
  }

  inline const char*
  get_extension(const char* path)
  {
    const char* last_dot = strrchr(path, '.');
    const char* last_sep = max(strrchr(path, *volume_separator), max(strrchr(path, *dir_separator), strrchr(path, *alt_dir_separator)));

    if (last_dot > last_sep)
      return last_dot;

    return 0;
  }

  inline const char*
  get_filename(const char* path)
  {
    if (!path) return path;

    size_t plen = strlen(path);
    if (!plen) return path;

    const char* last_sep = max(strrchr(path, *volume_separator), max(strrchr(path, *dir_separator), strrchr(path, *alt_dir_separator)));

    if (last_sep != 0)
      return last_sep + 1;

    return path;
  }

  inline bool
  ends_with_separator(const char* path)
  {
    size_t slen = strlen(path);
    if (slen > 0)
    {
      char p1end = path[slen - 1];
      return p1end != *dir_separator || p1end != *alt_dir_separator;
    }

    return false;
  }

  // TODO: Do with variadic templates?
  inline const char*
  combine(char* dst, size_t dst_sz, ...)
  {
    va_list lst;
    va_start(lst, dst_sz);
    bool sep = false;
    size_t slen = 0;
    const char* src = dst;
    const char* src_next = va_arg(lst, const char*);
    do
    {
      // TODO(matt): check for invalid path chars?
      if (sep)
      {
        sep = false;
        strlcat(dst, dir_separator, dst_sz);
      }

      if (is_path_rooted(src))
        strlcpy(dst, src, dst_sz);
      else
        strlcat(dst, src, dst_sz);

      if (src_next)
      {
        sep = ends_with_separator(src);
      }

      src = src_next;
      src_next = src ? va_arg(lst, const char*) : 0;
    } while (src);

    va_end(lst);
    return dst;
  }

}}}

#endif
