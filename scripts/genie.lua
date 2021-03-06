

ASTRO_DIR = path.getabsolute("..")
if BUILD_DIR == nil then
  BUILD_DIR = path.join(ASTRO_DIR, ".build")
  solution "astro"
end
ASTRO_THIRD_PARTY_DIR = path.join(ASTRO_DIR, "lib")

  configurations {
    "Debug",
    "Release"
  }

  platforms {
    "x32",
    "x64",
    "Native"
  }

  language "C++"
  startproject "astro.tests"

dofile "toolchain.lua"
toolchain(BUILD_DIR, ASTRO_THIRD_PARTY_DIR)

function copyLib()
end

dofile "astro.lua"

project "astro.tests"
  uuid (os.uuid("astro-tests"))
  kind "ConsoleApp"

  debugdir (path.join(ASTRO_DIR, "test"))

  removeflags {
    "NoExceptions"
  }

  links {
    "astro",
  }

  includedirs {
    path.join(ASTRO_DIR, "include"),
    ASTRO_THIRD_PARTY_DIR,
  }

  files {
    path.join(ASTRO_DIR, "test/main.cpp"),
  }

  configuration { "vs*" }

  configuration { "android*" }
    kind "ConsoleApp"
    targetextension ".so"
    linkoptions {
      "-shared"
    }

  configuration { "nacl or nacl-arm" }
    kind "ConsoleApp"
    targetextension ".nexe"
    links {
      "ppapi",
      "pthread"
    }

  configuration { "pnacl" }
    kind "ConsoleApp"
    targetextension ".pexe"
    links {
      "ppapi",
      "pthread"
    }

  configuration { "linux-*" }
    links {
      "pthread"
    }

  configuration { "osx" }
    links {
      "Cocoa.framework"
    }

  configuration {}

  strip()
