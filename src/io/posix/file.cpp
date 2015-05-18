#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace astro { namespace io
{
  bool
  file::exists(const char* path)
  {
    struct stat s;
    return stat(path, &s) == 0 && (S_ISREG(s.st_mode) || S_ISLNK(s.st_mode));
  }

  template<typename Allocator> const char*
  file::read_all_text(const char* path, Allocator alloc)
  {
    const char* contents = nullptr;
    if (exists(path))
    {
      FILE* f = fopen(path, "r");
      fseek(f, 0, SEEK_END);
      long len = ftell(f);

      contents = alloc.allocate(len + 1);
      fseek(f, 0, SEEK_SET);
      if (fread((void*)contents, sizeof(char), len, f) < len)
      {
        alloc.deallocate(contents, len + 1);
        contents = nullptr;
      }
    }

    return contents;
  }
}}
