-- Procedurally Generated Transitional Audio Build --

-- A solution contains projects, and defines the available configurations
solution "track_writer"
   includedirs {
       "/usr/local/include/",
       "../pgta_engine/"
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
   configuration "x32"
      targetsuffix "_x32"
   configuration "x64"
      targetsuffix "_x64"
   configuration {}

   -- A project defines one build target
   project "track_writer"
      kind "ConsoleApp"
      targetdir "../../bin"
      language "C++"
      linkoptions { "-v"}
      links {  "protobuf" }
      files { "../pgta_engine/Track.pb.h", "../pgta_engine/Track.pb.cc", "track_writer/**.h", "track_writer/**.cpp" }
      buildoptions { "-std=c++11" }

