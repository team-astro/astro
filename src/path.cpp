/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#include <astro/astro.h>
#include <astro/io/path.h>

namespace astro { namespace io { namespace path
{
# if ASTRO_PLATFORM_WIN32 || ASTRO_PLATFORM_WINRT
  const char* volume_separator = ":";
  const char* path_separator = ";";
  const char* dir_separator = "\\";
  const char* alt_dir_separator = "/";
  const char* invalid_filename_chars[] = {"\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
          "\x08", "\x09", "\x0A", "\x0B", "\x0C", "\x0D", "\x0E", "\x0F", "\x10", "\x11", "\x12",
          "\x13", "\x14", "\x15", "\x16", "\x17", "\x18", "\x19", "\x1A", "\x1B", "\x1C", "\x1D",
          "\x1E", "\x1F", "\x22", "\x3C", "\x3E", "\x7C", ":", "*", "?", "\\", "/", 0};
  const char* invalid_path_chars[] = {"\x22", "\x3C", "\x3E", "\x7C", "\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
          "\x08", "\x09", "\x0A", "\x0B", "\x0C", "\x0D", "\x0E", "\x0F", "\x10", "\x11", "\x12",
          "\x13", "\x14", "\x15", "\x16", "\x17", "\x18", "\x19", "\x1A", "\x1B", "\x1C", "\x1D",
          "\x1E", "\x1F", 0};
# else
  const char* volume_separator = "/";
  const char* path_separator = ":";
  const char* dir_separator = "/";
  const char* alt_dir_separator = "/";
  const char* invalid_filename_chars[] = {"\x00", "/", 0};
  const char* invalid_path_chars[] = {"\x00", 0};
# endif

}}}
