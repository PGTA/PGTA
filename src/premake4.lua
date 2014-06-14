-- Procedurally Generated Transitional Audio Build --

-- A solution contains projects, and defines the available configurations
solution "pgta_engine"
   includedirs { 
       path.getabsolute("../../OALWrapper/include/"),
       "/usr/local/include/SDL2/"
    }
   libdirs { 
       path.getabsolute("../../OALWrapper/build/")
    }
   location "build"
   --startproject "pgta_engine"
   language "C++"
   platforms { "x64", "x32" }
 	-- build configurations --
   configurations { "Debug", "Release" }
   configuration "Debug"
      targetdir "build/Debug"
   	defines { "DEBUG", "_DEBUG" }
      flags { "Symbols" }
   configuration "Release"
   	targetdir "build/Release"
   	defines { "NDEBUG" }
   	optimize "Full"
   	flags{ "MultiProcessorCompile" }
   configuration "x32"
      targetsuffix "_x32"
   configuration "x64"
      targetsuffix "_x64"
   configuration {}

   -- A project defines one build target
   project "engine"
      kind "ConsoleApp"
      targetdir "../bin"
      language "C++"
      linkoptions { "-v", "-framework OpenAL"}
      links { "OALWrapper", "ogg", "vorbisfile", "SDLmain", "SDL", "SDL2" }
      files { "pgta_engine/**.h", "pgta_engine/**.cpp" }
      buildoptions { "-I/System/Library/Frameworks/OpenAL.framework/Headers",  "-std=c++11" }
