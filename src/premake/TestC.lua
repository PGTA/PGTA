
local testname = ...

target_dir = path.getabsolute("../../bin/") .. "/"
local sdl2_dir = (path.getabsolute("../../../SDKs/") .. "/SDL2-2.0.3/")

project(testname)
    kind "ConsoleApp"
    language "C"
    debugdir "../.."
    debugcommand "%{cfg.buildtarget.directory}../../../bin/%{cfg.buildtarget.name}"
    includedirs "../PGTA"
    links "PGTALib"
    defines "SDL_MAIN_HANDLED"
    files
    {
       "../tests/"..testname.."/**.h",
       "../tests/"..testname.."/**.c"
    }

    filter "platforms:x32"
        targetsuffix "_x32"
    filter "platforms:x64"
        targetsuffix "_x64"

    filter "system:macosx or system:linux"
        includedirs "/usr/local/include/SDL2/"
        links "SDL2"
    filter {}

    postbuildcommands
    {
        "{MKDIR} ../../../bin",
        "{COPY} %{cfg.buildtarget.name} ../../../bin/"
    }

    if (os.get() == "windows") then
        -- include sdl2
        copy_sdl_dll = true
        dofile (sdl2_dir .. "premake5_include.lua")
    end

project "*"
