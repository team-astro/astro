#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <astro/io/file.h>

namespace astro { namespace io
{
  bool
  file::exists(const char* path)
  {
    struct stat s;
    return stat(path, &s) == 0 && (S_ISREG(s.st_mode) || S_ISLNK(s.st_mode));
  }

  const char*
  file::read_all_text(const char* path, allocator* alloc)
  {
    const char* contents = nullptr;
    if (exists(path))
    {
      FILE* f = fopen(path, "r");
      fseek(f, 0, SEEK_END);
      unsigned long len = ftell(f);

      contents = (const char*) ASTRO_ALLOC(alloc, len + 1);
      fseek(f, 0, SEEK_SET);
      if (fread((void*)contents, sizeof(char), len, f) < len)
      {
        ASTRO_FREE(alloc, (void*)contents);
        contents = nullptr;
      }
    }

    return contents;
  }
}}
