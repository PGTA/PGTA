
project "PGTALib"
    kind "SharedLib"
    includedirs "../PGTA"
    defines "PGTA_EXPORTS"
    dependson "FlatbufCompiler"
    files
    {
        "../PGTA/**.h",
        "../PGTA/**.cpp",
        "../PGTA/**.inl"
    }
    run_include("flatbuffers_include.lua", "flatbuffers")

    filter "system:windows"
        postbuildcommands{
            MKDIR("../../../bin/"),
            COPY("libPGTALib.dll", "../../../bin/libPGTALib.dll")
        }
    filter "system:linux"
        postbuildcommands{
            MKDIR("../../../bin/"),
            COPY("libPGTALib.so", "../../../bin/libPGTALib.so")
        }
    filter "system:macosx"
        postbuildcommands{
            MKDIR("../../../bin/"),
            COPY("libPGTALib.dylib", "../../../bin/libPGTALib.dylib")
        }
    filter {}
project "*"
