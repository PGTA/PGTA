
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

    postbuildcommands
    {
        "{MKDIR} ../../../bin",
        "{COPY} %{cfg.buildtarget.name} ../../../bin/"
    }
project "*"
