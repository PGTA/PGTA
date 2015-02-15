
project "PGTALib"
    targetdir "../../bin"
    kind "SharedLib"
    includedirs "../PGTA"
    defines "PGTA_EXPORTS"
    dependson "SchemaCompiler"
    files
    {
        "../PGTA/**.h",
        "../PGTA/**.cpp",
        "../PGTA/**.inl"
    }
    run_include("flatbuffers_include.lua", "flatbuffers")
project "*"
