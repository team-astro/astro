project "astro"
	uuid (os.uuid("astro"))
	kind "StaticLib"

	-- configuration { "osx or ios" }
	-- 	-- OSX ar doesn't like creating archive without object files
	-- 	-- here is object file...
	-- 	prebuildcommands {
	-- 		"@echo \"void dummy() {}\" > /tmp/dummy.cpp",
	-- 	}
	-- 	files {
	-- 		"/tmp/dummy.cpp",
	-- 	}

	configuration { "windows" }
		files {
			path.join(ASTRO_DIR, "src/**/win32/**.cpp"),
		}

	configuration { "not windows" }
		files {
			path.join(ASTRO_DIR, "src/**/posix/**.cpp"),
		}

	configuration {}
		files {
			path.join(ASTRO_DIR, "src/*.cpp"),
			path.join(ASTRO_DIR, "src/net/*.cpp"),
			path.join(ASTRO_THIRD_PARTY_DIR, "http-parser/http_parser.c"),
		}

  includedirs {
		path.join(ASTRO_DIR, "include"),
		path.join(ASTRO_DIR, "thirdparty"),
  }
  defines {
    "ASTRO_IMPLEMENTATION"
  }

	files {
		path.join(ASTRO_DIR, "include/**.h"),
	}
