/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

using namespace astro;
using namespace astro::io;

TEST_CASE("Path combining works", "[io.path]") {
#if ASTRO_PLATFORM_WIN32 || ASTRO_PLATFORM_WINRT
  const char* abspath = "c:\\foo\\test.doc";
  const char* dirname = "c:\\foo";
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
  REQUIRE(strcmp(abspath, buffer) == 0);

  REQUIRE(strcmp(ext, path::get_extension(abspath)) == 0);
  REQUIRE(strcmp(ext, path::get_extension(filename)) == 0);
  REQUIRE(strcmp(filename, path::get_filename(abspath)) == 0);
}
