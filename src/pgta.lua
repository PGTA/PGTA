-- Procedurally Generated Transitional Audio Build --

target_dir = path.getabsolute("../bin/") .. "/"

local sdks_dir = (path.getabsolute("../../SDKs/") .. "/")
local sdl2_dir = (sdks_dir .. "SDL2-2.0.3/")

local function run_include(script, rel_dir)
    local script_full = "external/build-tools/premake_scripts/" .. script
    local incl_prefix = iif(string.find(_ACTION, "vs20"), "$(ProjectDir)", "")
    local module_full = "../external/" .. rel_dir
    assert(loadfile(script_full))(incl_prefix, module_full)
end

-- A solution contains projects, and defines the available configurations
solution "PGTA"
    location "build"
    startproject "engine"
    targetdir "../bin"
    
    language "C++"
    platforms { "x64", "x32" }
    configurations { "Debug", "Release" }
    flags
    {
        "Symbols",
        "StaticRuntime",
        "NoMinimalRebuild",
        "NoEditAndContinue",
        "MultiProcessorCompile"
    }
    
    filter "action:vs*"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE",
            "SDL_MAIN_HANDLED"
        }
    filter "system:macosx or system:linux"
        buildoptions "-std=c++11"
        
    filter "platforms:x32"
        targetsuffix "_x32"
    filter "platforms:x64"
        targetsuffix "_x64"
        
    filter "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter "Release"
        flags "LinkTimeOptimization"
        defines "NDEBUG"
        optimize "Full"
    
    filter "files:**.fbs"
        buildmessage "flatc: Compiling %{file.relpath}"
        buildcommands
        {
            
        }
        buildoutputs "%{cfg.objdir}/%{file.basename}.h"
    filter {}
    
    group "external"
        run_include("flatbuffers.lua", "flatbuffers")
    group ""
    
    project "PGTAschema"
        kind "None"
        files "PGTA/**.fbs"
        dependson "flatc"
    
    project "PGTA"
        kind "SharedLib"
        includedirs "PGTA"
        defines "PGTA_EXPORTS"
        files
        {
            "PGTA/**.h",
            "PGTA/**.cpp",
            "PGTA/**.inl"
        }
        run_include("flatbuffers_include.lua", "flatbuffers")

    -- A project defines one build target
    project "engine"
        kind "ConsoleApp"
        includedirs "PGTA"
        links "PGTA"
        files
        {
           "engine/**.h",
           "engine/**.cpp"
        }
        
        filter { "system:macosx or system:linux" }
            includedirs "/usr/local/include/SDL2/"
            links
            {
                "SDL2"
            }
        filter {}
        if (os.get() == "windows") then
            -- include sdl2
            copy_sdl_dll = true
            dofile (sdl2_dir .. "premake5_include.lua")
        end
