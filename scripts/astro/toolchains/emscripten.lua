module(..., package.seeall)

local native = require "tundra.native"
local platform = require "tundra.platform"
local util = require "tundra.util"
local path = require "tundra.path"

function apply(env, options)
  tundra.unitgen.load_toolset("clang", env)

  env:set_many {
    ["NATIVE_SUFFIXES"] = { ".c", ".cpp", ".cc", ".cxx", ".a", ".o", ".bc" },
    ["OBJECTSUFFIX"] = ".bc",
    ["LIBSUFFIX"] = ".bc",
    ["PROGSUFFIX"] = ".html",
    ["CC"] = "emcc",
    ["CXX"] = "em++",
    ["LIB"] = "em++",
    ["LD"] = "em++",
    ["LIBCOM"] = "$(LIB) $(LIBOPTS) -o $(@) $(<)",
  }
end
