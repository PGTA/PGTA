
target_dir = path.getabsolute("../../bin/") .. "/"
local sdl2_dir = (path.getabsolute("../../../SDKs/") .. "/SDL2-2.0.3/")

project "TestEngine"
    targetdir "../../bin"
    kind "ConsoleApp"
    includedirs "../PGTA"
    links "PGTALib"
    defines "SDL_MAIN_HANDLED"
    files
    {
       "../engine/**.h",
       "../engine/**.cpp"
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
project "*"
