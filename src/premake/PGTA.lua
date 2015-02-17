
-- Procedurally Generated Transitional Audio Build --

function run_include(script, rel_dir)
    local script_full = "../external/build-tools/premake_scripts/" .. script
    local incl_prefix = iif(string.find(_ACTION, "vs20"), "$(ProjectDir)../", "")
    local module_full = "../external/" .. rel_dir
    assert(loadfile(script_full))(incl_prefix, module_full)
end

function TOUCH(file)
    if(os.get() == "windows") then
        return "type nul >> "..file.." && copy /b "..file.."+,, "..file
    else
        return "touch "..file
    end
end

function COPY(source, destination)
    if(os.get() == "windows") then
        return "xcopy /Q /E /Y /I "..source.." "..destination
    else
        return "cp -rf "..source.." "..destination
    end
end

function MOVE(source, destination)
    if(os.get() == "windows") then
        return "move /Y "..source.." "..destination
    else
        return "mv -f "..source.." "..destination
    end
end

function MKDIR(dirpath)
    if(os.get() == "windows") then
        return "mkdir "..dirpath
    else
        return "mkdir -p "..dirpath
    end
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

    group "external"
        run_include("flatbuffers.lua", "flatbuffers")
    group ""

    dofile "SchemaCompiler.lua"
    dofile "PGTALib.lua"
    assert(loadfile("Test.lua"))("AudioMixerTest")
    assert(loadfile("Test.lua"))("EngineTest")
