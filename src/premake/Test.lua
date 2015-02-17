
local testname = ...

target_dir = path.getabsolute("../../bin/") .. "/"
local sdl2_dir = (path.getabsolute("../../../SDKs/") .. "/SDL2-2.0.3/")

project(testname)
    kind "ConsoleApp"
    includedirs { "../PGTA", "../tests/Common" }
    links "PGTALib"
    defines "SDL_MAIN_HANDLED"
    files
    {
       "../tests/"..testname.."/**.h",
       "../tests/"..testname.."/**.cpp",
       "../tests/Common/**.h",
       "../tests/Common/**.cpp"
    }

    filter "platforms:x32"
        targetsuffix "_x32"
    filter "platforms:x64"
        targetsuffix "_x64"

    filter "system:macosx or system:linux"
        includedirs "/usr/local/include/SDL2/"
        links "SDL2"
    filter {}
    if (os.get() == "windows") then
        -- include sdl2
        copy_sdl_dll = true
        dofile (sdl2_dir .. "premake5_include.lua")
    end
    postbuildcommands{
        MKDIR("../../../bin/"),
        COPY(testname.."_*", "../../../bin/")
    }
project "*"
