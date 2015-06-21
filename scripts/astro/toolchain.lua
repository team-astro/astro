-- toolchain.lua - common configuration for astro Projects

module(..., package.seeall)

-- Exports
config = {}
xcode_base = ""

local util = require "tundra.util"
local astro = require "astro.util"

xcode_base = astro.exec_and_read_line("xcode-select -p")

-- OS X Support
local osx_sdk = "MacOSX10.10"
local osx_platform_base = xcode_base .. "/Platforms/MacOSX.platform"
local osx_sysroot = osx_platform_base .. "/Developer/SDKs/" .. osx_sdk .. ".sdk"

local osx_opts = {
  "-isysroot " .. osx_sysroot,
  "-arch i386 -arch x86_64",
}
local osx_config = {
  Tools = { "clang-osx" },
  Env = {
    CXXOPTS = {
      "-std=c++11 -stdlib=libc++"
    },
    PROGOPTS = {
      "-stdlib=libc++"
    },
    SHLIBOPTS = {
      "-stdlib=libc++"
    },
  },
  ReplaceEnv = {
    -- link with c++ compiler to get stdlib
    LD = "$(CXX)",
  },
}

config = util.merge_arrays(config, {
  Config {
    Name = "osx-clang",
    DefaultOnHost = "macosx",
    Inherit = osx_config,
    Env = {
      CXXOPTS = osx_opts,
      PROGOPTS = osx_opts,
      SHLIBOPTS = osx_opts,
    }
  },
})

-- iOS Support
local ios_sdk = "8.3"
local ios_deployment_target = "8.0"
local ios_platform_base = xcode_base .. "/Platforms/iPhoneOS.platform"
local ios_sysroot = ios_platform_base .. "/Developer/SDKs/iPhoneOS" .. ios_sdk .. ".sdk"
local ios_sim_platform_base = xcode_base .. "/Platforms/iPhoneSimulator.platform"
local ios_sim_sysroot = ios_sim_platform_base .. "/Developer/SDKs/iPhoneSimulator" .. ios_sdk .. ".sdk"

local ios_opts = {
  "-miphoneos-version-min=" .. ios_deployment_target,
  "-isysroot " .. ios_sysroot,
  "-arch armv7 -arch armv7s -arch arm64",
}

local ios_sim_opts = {
  "-mios-simulator-version-min=" .. ios_deployment_target,
  "-isysroot " .. ios_sim_sysroot,
  "-arch i386 -arch x86_64",
}

config = util.merge_arrays(config, {
  Config {
    Name = "ios-clang",
    SupportedHosts = { "macosx" },
    Inherit = osx_config,
    Env = {
      CXXOPTS = ios_opts,
      PROGOPTS = ios_opts,
      SHLIBOPTS = ios_opts,
    },
  },
  Config {
    Name = "ios_simulator-clang",
    SupportedHosts = { "macosx" },
    Inherit = osx_config,
    Env = {
      CXXOPTS = ios_sim_opts,
      PROGOPTS = ios_sim_opts,
      SHLIBOPTS = ios_sim_opts,
    },
  },
})

-- Win32 Support
local win32_config = {
  Env = {
    CXXOPTS = {
      { "/W4"; Config = "*-*-*" },
      { "/O2"; Config = "*-*-release" },
    },
    GENERATE_PDB = {
      { "0"; Config = "*-*-release" },
      { "1"; Config = { "*-*-debug", "*-*-production" } },
    },
  },
}

config = util.merge_arrays(config, {
  Config {
    Name = 'win64-vs2015',
    Tools = { { "msvc-vs2015"; TargetArch = "x64" }, },
    DefaultOnHost = "windows",
    Inherit = win32_config,
  },
  Config {
    Name = 'win32-vs2015',
    Tools = { { "msvc-vs2015"; TargetArch = "x32" }, },
    SupportedHosts = { "windows" },
    Inherit = win32_config,
  },
})

-- asm.js Support
local asmjs_config = {
  Env = {
    CXXOPTS = {
      "-std=c++11",
      { "-s ASSERTIONS=2 -s SAFE_HEAP=1"; Config = "asmjs-debug-*" },
    },
    PROGOPTS = {
      { "-s ASSERTIONS=2 -s SAFE_HEAP=1"; Config = "asmjs-debug-*" },
    },
  }
}

config = util.merge_arrays(config, {
  Config {
    Name = 'asmjs-browser',
    Tools = { { "astro.toolchains.emscripten" } },
    SupportedHosts = { "windows", "macosx", "linux" },
    Inherit = asmjs_config,
  }
})

-- NaCl Support

local nacl_config = {
  Env = {
    CXXOPTS = {
      "-std=c++11 -stdlib=libc++"
    },
    PROGOPTS = {
      "-stdlib=libc++"
    },
    SHLIBOPTS = {
      "-stdlib=libc++"
    },
  },
  ReplaceEnv = {
    -- link with c++ compiler to get stdlib
    LD = "$(CXX)",
  },
  Libs = {
    "ppapi",
    "ppapi_cpp"
  }
}

config = util.merge_arrays(config, {
  Config {
    Name = 'nacl-portable',
    Tools = { { "astro.toolchains.nacl"; Toolchain = "pnacl" } },
    SupportedHosts = { "windows", "macosx", "linux" },
    Inherit = nacl_config,
    -- Env = pnacl_config.Env,
    -- ReplaceEnv = pnacl_config.ReplaceEnv,
  }
})
