
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
            COPY("PGTALib.dll", "../../../bin/")
        }
    filter "system:linux"
        postbuildcommands{
            MKDIR("../../../bin/"),
            COPY("libPGTALib.so", "../../../bin/")
        }
    filter "system:macosx"
        postbuildcommands{
            MKDIR("../../../bin/"),
            COPY("libPGTALib.dylib", "../../../bin/")
        }
    filter {}
project "*"
