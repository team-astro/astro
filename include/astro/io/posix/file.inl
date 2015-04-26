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

  const char*
  file::read_all_text(const char* path, allocator alloc)
  {
    char* contents = nullptr;
    if (exists(path))
    {
      FILE* f = fopen(path, "r");
      fseek(f, 0, SEEK_END);
      long len = ftell(f);

      contents = (char*)alloc.allocate(len);
      fseek(f, 0, SEEK_SET);
      if (fread(contents, sizeof(char), len, f) < len)
      {
        alloc.deallocate(contents);
        contents = nullptr;
      }
    }

    return contents;
  }
}}
