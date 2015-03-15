
project "PGTALib"
    kind "SharedLib"
    defines "PGTA_EXPORTS"
    dependson "FlatbufCompiler"
    links "AudioMixer"
    includedirs
    {
        "../AudioMixer/include/",
        "../PGTA/include"
    }
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
