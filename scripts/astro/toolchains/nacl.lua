module(..., package.seeall)

local native = require "tundra.native"
local platform = require "tundra.platform"
local util = require "tundra.util"
local path = require "tundra.path"

local valid_arches = { "arm", "x86_32", "x86_64", "i686" }
local valid_pnacl_arches = { nil, "pnacl" }
local arch_name = {
  ["arm"] = "arm",
  ["x86_32"] = "i686",
  ["i686"] = "i686",
  ["x86_64"] = "x86_64",
}

local arch_alt_name = {
  ["arm"] = "arm",
  ["x86_32"] = "x86_32",
  ["i686"] = "x86_32",
  ["x86_64"] = "x86_64"
}

local arch_base_name = {
  ["arm"] = "arm",
  ["x86_32"] = "x86",
  ["i686"] = "x86",
  ["x86_64"] = "x86",
}

local nacl_toolchains = { "newlib", "glibc", "pnacl", "bionic", "clang-newlib" }
local host_toolchains = { "linux", "mac", "win" }
local valid_toolchains = { "host" }
util.append_table(valid_toolchains, host_toolchains)
util.append_table(valid_toolchains, nacl_toolchains)

local clang_tools = {
  ["cc"] = "clang",
  ["c++"] = "clang++",
  ["gcc"] = "clang",
  ["g++"] = "clang++",
  ["ld"] = "clang++",
}

local gcc_tools = {
  ["cc"] = "gcc",
  ["c++"] = "g++",
  ["gcc"] = "gcc",
  ["g++"] = "g++",
  ["ld"] = "g++",
}

local platform_to_nacl = {
  ["macosx"] = "mac",
  ["linux"] = "linux",
  ["windows"] = "win",
  ["freebsd"] = "linux",
  ["openbsd"] = "linux",
}

local function canonicalize_toolchain(toolchain)
  if toolchain == "host" then
    return platform_to_nacl[platform.host_platform()]
  end
  return toolchain
end

local function get_sdk_path()
  return native.getenv("NACL_SDK_ROOT")
end

local function get_toolchain_dir(toolchain, arch)
  local sdk_root = native.getenv("NACL_SDK_ROOT")
  local platform = platform_to_nacl[platform.host_platform()]
  local pnacl_tools = util.make_lookup_table({ "pnacl", "clang-newlib" })

  local subdir = nil
  if pnacl_tools[toolchain] then
    subdir = platform .. "_pnacl"
  else
    local base_name = arch_base_name[arch]
    subdir = platform .. "_" .. base_name .. "_" .. toolchain
  end


  local result = path.join(path.join(sdk_root, "toolchain"), subdir)
  -- print("get_toolchain_dir", toolchain, arch, result)
  return result
end

local function get_toolchain_arch_dir(toolchain, arch)
  local toolchain_dir = get_toolchain_dir(toolchain, arch)
  local arch_dir = arch_name[arch]
  local result = path.join(toolchain_dir, arch_dir)
  -- print("get_toolchain_arch_dir", toolchain, arch, result)
  return result
end

local function get_toolchain_bin_dir(toolchain, arch)
  local result = path.join(get_toolchain_dir(toolchain, arch), "bin")
  -- print("get_toolchain_bin_dir", toolchain, arch, result)
  return result
end

local function get_sdk_include_dirs(toolchain)
  local base_include = path.join(get_sdk_path(), "include")
  if toolchain == "clang-newlib" then
    toolchain = "newlib"
  end
  return { base_include, path.join(base_include, toolchain) }
end

local function get_sdk_lib_dir()
  return path.join(get_sdk_path(), "lib")
end

local function expect(condition, message)
  if not condition then
    error(message, 2)
  end
end

local function expect_toolchain(toolchain, expected_toolchains)
  expect(util.array_contains(expected_toolchains, toolchain),
    "Expected toolchain to be one of [" .. table.concat(expected_toolchains, ", ") .. "], not " .. toolchain)
end

local function expect_arch(arch, expected_arches)
  expect(util.array_contains(expected_arches, arch),
    "Expected arch to be one of [" .. table.concat(expected_arches, ", ") .."], not " .. arch)
end

local function check_valid_toolchain_arch(toolchain, arch, arch_required)
  arch_required = arch_required == true or false

  if toolchain or arch or arch_required then
    expect_toolchain(toolchain, valid_toolchains)
  end

  -- TODO: Format error strings only if condition fails
  if util.array_contains(host_toolchains, toolchain) then
    expect(arch == nil,
      "Expected no arch for host toolchain " .. toolchain .. ".")
  elseif toolchain == "pnacl" then

    expect((arch == nil) or (arch == "pnacl"),
      "Expected no arch for toolchain " .. toolchain .. ".")
  elseif arch_required then
    expect(arch ~= nil,
      "Expected arch to be one of [" .. table.concat(valid_arches, ", ") .. "] for toolchain " .. toolchain .. ".\n")
  end

  if arch then
    if toolchain == "pnacl" then
      expect_arch(arch, valid_pnacl_arches)
    else
      expect_arch(arch, valid_arches)
    end

    if arch == "arm" then
      expect(util.array_contains({ "newlib", "bionic", "clang-newlib" }, toolchain),
        "The arm arch only supports newlib.")
    end
  end
end

-- Commands
local function get_tool_path(toolchain, arch, tool)
  if tool == "gdb" then
    return path.join(get_toolchain_bin_dir("newlib", "x86_64"), "x86_64-nacl-gdb")
  end

  local full_tool_name = nil
  if toolchain == "pnacl" then
    check_valid_toolchain_arch(toolchain, arch)
    if clang_tools[tool] then
      tool  = clang_tools[tool]
    end
    full_tool_name = "pnacl-" .. tool
  else
    check_valid_toolchain_arch(toolchain, arch, true)
    if toolchain == "clang-newlib" then
      if clang_tools[tool] ~= nil then
        tool = clang_tools[tool]
      end
    else
      if gcc_tools[tool] ~= nil then
        tool = gcc_tools[tool]
      end
    end

    full_tool_name = arch_name[arch] .. "-nacl-" .. tool
  end

  local result = path.join(get_toolchain_bin_dir(toolchain, arch), full_tool_name)
  -- print("get_tool_path", toolchain, arch, tool, result)
  return result
end

local function get_cflags(toolchain)
  local includes = util.map(get_sdk_include_dirs(toolchain), function (dirname)
    return "-I" .. dirname
  end)

  return table.concat(includes, " ")
end

local function get_include_dirs(toolchain)
  return table.concat(get_sdk_include_dirs(toolchain), " ")
end

local function get_ldflags()
  return "-L" .. get_sdk_lib_dir()
end

function apply(env, options)
  -- print("apply called!", env, options)
  tundra.unitgen.load_toolset("clang", env)

  options.Toolchain = options.Toolchain or native.getenv("NACL_TOOLCHAIN", "pnacl")
  options.TargetArch = options.TargetArch or native.getenv("NACL_ARCH", nil)
  options.Toolchain = canonicalize_toolchain(options.Toolchain)

  check_valid_toolchain_arch(options.Toolchain, options.TargetArch)

  -- local e = {
  --   ["CC"] = get_tool_path(options.Toolchain, options.TargetArch, "cc"),
  --   ["CXX"] = get_tool_path(options.Toolchain, options.TargetArch, "c++"),
  --   ["LIB"] = get_tool_path(options.Toolchain, options.TargetArch, "ar"),
  --   ["LD"] = get_tool_path(options.Toolchain, options.TargetArch, "ld"),
  --   ["CPPPATH"] = get_sdk_include_dirs(options.Toolchain),
  --   ["LIBPATH"] = get_sdk_lib_dir(),
  -- }
  --
  -- print(util.tostring(e))

  env:set_many {
    ["CC"] = get_tool_path(options.Toolchain, options.TargetArch, "cc"),
    ["CXX"] = get_tool_path(options.Toolchain, options.TargetArch, "c++"),
    ["LIB"] = get_tool_path(options.Toolchain, options.TargetArch, "ar"),
    ["LD"] = get_tool_path(options.Toolchain, options.TargetArch, "ld"),
    ["CPPPATH"] = get_sdk_include_dirs(options.Toolchain),
    ["LIBPATH"] = get_sdk_lib_dir(),
  }
end
