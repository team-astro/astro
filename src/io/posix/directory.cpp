#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <fcntl.h>

#include <astro/io/directory.h>

namespace astro { namespace io
{
  bool
  directory::exists(const char* path)
  {
    struct stat s;
    return stat(path, &s) == 0 && S_ISDIR(s.st_mode);
  }
}}
