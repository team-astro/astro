/**
* Copyright 2015 Team Astro. All rights reserved.
*/

using namespace astro;
using namespace astro::io;

TEST path_combining_works() {
#if ASTRO_PLATFORM_WIN32 || ASTRO_PLATFORM_WINRT
  const char* abspath = "c:\\foo\\test.doc";
  const char *dirname = "c:\\foo";
  const char* filename = "test.doc";
  const char* ext = ".doc";
#else
  const char* abspath = "/foo/test.txt";
  const char* dirname = "/foo";
  const char* filename = "test.txt";
  const char* ext = ".txt";
#endif

  char buffer[64] = {};
  astro::strlcpy(buffer, dirname, 64);
  path::combine(buffer, 64, filename, 0);
  ASSERT_EQ(0, strcmp(abspath, buffer));

  ASSERT_EQ(0, strcmp(ext, path::get_extension(abspath)));
  ASSERT_EQ(0, strcmp(ext, path::get_extension(filename)));
  ASSERT_EQ(0, strcmp(filename, path::get_filename(abspath)));

  PASS();
}

SUITE(path_tests) { RUN_TEST(path_combining_works); }
