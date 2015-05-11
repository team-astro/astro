project "astro"
	uuid (os.uuid("astro"))
	kind "StaticLib"

	configuration { "osx or ios" }
		-- OSX ar doesn't like creating archive without object files
		-- here is object file...
		prebuildcommands {
			"@echo \"void dummy() {}\" > /tmp/dummy.cpp",
		}
		files {
			"/tmp/dummy.cpp",
		}

	configuration {}

  includedirs {
    path.getabsolute("../include")
  }
  defines {
    "ASTRO_IMPLEMENTATION"
  }

	files {
		"../include/**.h",
	}
