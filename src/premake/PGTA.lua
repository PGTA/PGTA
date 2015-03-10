
-- Procedurally Generated Transitional Audio Build --

include "compilationunit.lua"

function run_include(script, rel_dir)
    local external_dir = path.getabsolute("../external")
    local repo_dir = path.join(external_dir, rel_dir)
    local script_full = external_dir.."/build-tools/premake_scripts/"..script
    local output_dir = path.getabsolute("../premake/".._ACTION)
    assert(loadfile(script_full))(repo_dir, output_dir)
end

solution "PGTA"
    location(_ACTION)
    targetdir(_ACTION)
    startproject "EngineTest"

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
    warnings "Extra"

    filter "action:vs*"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE"
        }
    filter { "action:not vs*" }
        buildoptions "-fPIC"
        compilationunitdir(_ACTION.."/%{prj.name}Dir")
    filter { "action:not vs*", "language:C++" }
        buildoptions "-std=c++1y"
    filter { "action:not vs*", "language:C" }
        buildoptions "-std=c11"

    filter "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter "Release"
        flags "LinkTimeOptimization"
        defines "NDEBUG"
        optimize "Full"
    filter {}

    group "Externals"
        run_include("flatbuffers.lua", "flatbuffers")
    group "Externals"
        run_include("flatc.lua", "flatbuffers")
    group "Externals"
        run_include("sdl2.lua", "SDL2")
    group "PGTA"
        dofile "AudioMixer.lua"
    group "PGTA"
        dofile "PGTALib.lua"
    group "BuildTools"
        dofile "SchemaCompiler.lua"
    group "Tests"
        assert(loadfile("Test.lua"))("AudioMixerTest", "ConsoleApp")
    group "Tests"
        assert(loadfile("Test.lua"))("EngineTest", "ConsoleApp")
    group "Tests"
        assert(loadfile("TestC.lua"))("CInterfaceTest")
    group "Tests"
        assert(loadfile("Test.lua"))("PGTATestCommon", "StaticLib")
    group ""
