/**
* Copyright 2015 Team Astro. All rights reserved.
*/

#ifndef ASTRO_LOGGING
#define ASTRO_LOGGING

#include <cstdio>
#include <errno.h>
#include <cstring>

namespace astro
{
  enum class log_level
  {
    none,
    error,
    warn,
    info,
    debug
  };
}

extern astro::log_level astro_log_verbosity;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/* safe readable version of errno */
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#ifdef ASTRO_LOGGING_NOCOLORS // runtime check?
  #define log_error(M, ...) if (astro_log_verbosity >= log_level::error) \
    fprintf(stderr,  "ERR   " M " at %s (%s:%d) errno:%s\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__, clean_errno())
  #define log_warn(M, ...) if (astro_log_verbosity >= log_level::warn) \
    fprintf(stderr, "WARN  " M " at %s (%s:%d) errno:%s\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__, clean_errno())
  #define log_info(M, ...) if (astro_log_verbosity >= log_level::info) \
    fprintf(stderr, "INFO  " M " at %s (%s:%d)\n", \
    ##__VA_ARGS__, __func__, __FILENAME__, __LINE__)
  #define log_debug(M, ...) if (astro_log_verbosity >= log_level::debug) \
    fprintf(stderr, "DEBUG " M " at %s (%s:%d) \n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__)
#else
  #define log_error(M, ...) if (astro_log_verbosity >= log_level::error) \
    fprintf(stderr,  "\33[31mERR\33[39m   " M "  \33[90m at %s (%s:%d) \33[94merrno: %s\33[39m\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__, clean_errno())
  #define log_warn(M, ...) if (astro_log_verbosity >= log_level::warn) \
    fprintf(stderr, "\33[91mWARN\33[39m  " M "  \33[90m at %s (%s:%d) \33[94merrno: %s\33[39m\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__, clean_errno())
  #define log_info(M, ...) if (astro_log_verbosity >= log_level::info) \
    fprintf(stderr, "\33[32mINFO\33[39m  " M "  \33[90m at %s (%s:%d) \33[39m\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__)
  #define log_debug(M, ...) if (astro_log_verbosity >= log_level::debug) \
    fprintf(stderr, "\33[34mDEBUG\33[39m " M "  \33[90m at %s (%s:%d) \33[39m\n", \
      ##__VA_ARGS__, __func__, __FILENAME__, __LINE__)
#endif /* ASTRO_LOGGING_NOCOLORS */

#endif // ASTRO_LOGGING
