-- Procedurally Generated Transitional Audio Build --

target_dir = path.getabsolute("../bin/") .. "/"

local sdks_dir = (path.getabsolute("../../SDKs/") .. "/")
local ogg_dir = (sdks_dir .. "libogg-1.3.2/")
local vorbis_dir = (sdks_dir .. "libvorbis-1.3.4/")
local oalwrapper_dir = (sdks_dir .. "OALWrapper/")
local oalsoft_dir = (sdks_dir .. "openal-soft-1.15.1/")
local sdl2_dir = (sdks_dir .. "SDL2-2.0.3/")
local proto_dir = (sdks_dir .. "protobuf-2.5.0/")

-- A solution contains projects, and defines the available configurations
solution "pgta_engine"
    location "build"
    startproject "pgta_engine"
    
    platforms { "x64", "x32" }
    configurations { "Debug", "Release" }
    flags { "Symbols" }
    
    filter { "action:vs*" }
        flags
        {
            "StaticRuntime", "NoMinimalRebuild", "NoEditAndContinue", "MultiProcessorCompile"
        }
    filter {}
    
    filter { "platforms:x32" }
        targetsuffix "_x32"
    filter { "platforms:x64" }
        targetsuffix "_x64"
    filter {}
    
    filter { "Release" }
        flags { "LinkTimeOptimization" }
        optimize "Full"
    filter {}

    -- A project defines one build target
    project "pgta_engine"
        kind "ConsoleApp"
        targetdir "../bin"
        language "C++"
        
        files
        {
            "pgta_engine/**.h",
            "pgta_engine/**.cpp",
            "pgta_engine/**.cc"
        }
        
        filter { "Debug" }
            defines { "DEBUG", "_DEBUG" }
        filter { "Release" }
            defines { "NDEBUG" }
        filter {}
        
        filter { "system:macosx" }
            buildoptions
            {
                "-std=c++11"
            }
            includedirs
            { 
                path.getabsolute("../../OALWrapper/include/"),
                "/usr/local/include/SDL2/"
            }
            libdirs
            { 
                path.getabsolute("../../OALWrapper/build/")
            }
            links
            {
                "OALWrapper", "ogg", "vorbisfile", "OpenAL", "SDLmain", "SDL", "SDL2"
            }
        filter {}
        
        -- include libogg
        dofile (ogg_dir .. "premake5_include.lua")
        
        -- include libvorbis
        dofile (vorbis_dir .. "premake5_include.lua")
        
        -- include OALWrapper
        dofile (oalwrapper_dir .. "premake5_include.lua")
        
        -- include openal-soft
        copy_oal_dll = true
        dofile (oalsoft_dir .. "premake5_include.lua")
        
        -- include sdl2
        copy_sdl_dll = true
        dofile (sdl2_dir .. "premake5_include.lua")
        
        -- include protobuf
        dofile (proto_dir .. "premake5_include.lua")
