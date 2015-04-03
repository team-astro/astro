/**
* Copyright 2015 Matt Rudder. All rights reserved.
*/

TEST_CASE("Path combining works", "[io.path]") {
#if MU_PLATFORM_WIN32 || MU_PLATFORM_WINRT
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
  mu::strlcpy(buffer, dirname, 64);
  mu::io::path::combine(buffer, 64, filename, 0);
  REQUIRE(strcmp(abspath, buffer) == 0);

  REQUIRE(strcmp(ext, mu::io::path::get_extension(abspath)) == 0);
  REQUIRE(strcmp(ext, mu::io::path::get_extension(filename)) == 0);
  REQUIRE(strcmp(filename, mu::io::path::get_filename(abspath)) == 0);
}
