require "tundra.syntax.glob"

StaticLibrary {
  Name = "Astro",
  Sources = {
    FGlob {
      Dir = _G.ASTRO_ROOT .. "src",
      Extensions = { ".cpp", ".mm" },
      Filters = {
        { Pattern = "/win32/"; Config = "win*-*-*" },
        { Pattern = "/posix/"; Config = { "osx-*-*", "nacl-*-*", "asmjs-*-*" } },
      },
    },
  },
  Frameworks = {
    { "Foundation"; Config = "osx-*-*" },
    { "Foundation"; Config = "ios*-*-*" },
  },
  Env = {
    CPPPATH = {
      _G.ASTRO_ROOT .. "include/",
      _G.ASTRO_ROOT .. "lib/",
    },
  },
  Propagate = {
    Frameworks = {
      { "Foundation"; Config = "osx-*-*" },
      { "Foundation"; Config = "ios*-*-*" },
    },
    Env = {
      CPPPATH = {
        _G.ASTRO_ROOT .. "include/",
        _G.ASTRO_ROOT .. "lib/",
      },
      PROGOPTS = {
      }
    },
  },
}
