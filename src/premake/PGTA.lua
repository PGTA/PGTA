
-- Procedurally Generated Transitional Audio Build --

function run_include(script, rel_dir)
    local script_full = "../external/build-tools/premake_scripts/" .. script
    local incl_prefix = iif(string.find(_ACTION, "vs20"), "$(ProjectDir)../../external/", "../../")
    local files_prefix = "../../"
    assert(loadfile(script_full))(incl_prefix, files_prefix, rel_dir)
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

    group "Externals"
        run_include("flatbuffers.lua", "flatbuffers")
    group "PGTA"
        dofile "AudioMixer.lua"
    group "PGTA"
        dofile "PGTALib.lua"
    group "BuildTools"
        dofile "SchemaCompiler.lua"
    group "Tests"
        assert(loadfile("Test.lua"))("AudioMixerTest")
    group "Tests"
        assert(loadfile("Test.lua"))("EngineTest")
    group ""
