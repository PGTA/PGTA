-- Procedurally Generated Transitional Audio Build --


-- A solution contains projects, and defines the available configurations
solution "pgta_engine"
   location "build"
   --startproject "pgta_engine"
   language "C++"
 
 	-- build configurations --
   configurations { "Debug", "Release" }
   configuration "Debug"
   		targetdir "build/Debug"
   		defines { "DEBUG", "_DEBUG" }
   	configuration "Release"
   		targetdir "build/Release"
   		defines { "NDEBUG" }
   		optimize "Full"
   		flags{ "LinkTimeOptimization", "MultiProcessorCompile" }
   	configuration {}

   -- A project defines one build target
   project "engine"
      kind "ConsoleApp"
	  targetdir "../bin"
      language "C++"
      files { "pgta_engine/**.h", "pgta_engine/**.cpp" }
	
	configuration "linux"
		buildoptions { "-std=c++11" }
	configuration{}
