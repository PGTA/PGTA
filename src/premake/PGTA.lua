
-- Procedurally Generated Transitional Audio Build --

function run_include(script, rel_dir)
    local script_full = "../external/build-tools/premake_scripts/" .. script
    local incl_prefix = iif(string.find(_ACTION, "vs20"), "$(ProjectDir)../", "")
    local module_full = "../external/" .. rel_dir
    assert(loadfile(script_full))(incl_prefix, module_full)
end

solution "PGTA"
    location(_ACTION)
    targetdir(_ACTION)
    startproject "TestEngine"

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
            "_CRT_NONSTDC_NO_DEPRECATE"
        }
    filter "system:macosx or system:linux"
        buildoptions { "-std=c++11", "-fPIC" }

    filter "Debug"
        defines { "DEBUG", "_DEBUG" }
    filter "Release"
        flags "LinkTimeOptimization"
        defines "NDEBUG"
        optimize "Full"
    filter {}

    group "external"
        run_include("flatbuffers.lua", "flatbuffers")
    group ""

    dofile "SchemaCompiler.lua"
    dofile "PGTALib.lua"
    assert(loadfile("Test.lua"))("AudioMixerTest")
    assert(loadfile("Test.lua"))("EngineTest")
