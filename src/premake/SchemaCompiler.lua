
local function runexe(name)
    if (os.get() == "windows") then
        return name
    else
        return './'..name
    end
end

filter "files:**.fbs"
    buildmessage "flatc: Compiling %{file.relpath}"
    buildcommands
    {
        runexe('flatc')..' -c -o "../../PGTA/public/schema" %{file.relpath}',
        runexe('SchemaCompiler')..' %{file.relpath}'
    }
filter { "files:**.fbs", "system:not windows" }
    buildoutputs { "%{file.name}.h", "%{file.basename}_generated.h" }
filter {}

project "SchemaCompiler"
    kind "ConsoleApp"
    run_include("flatc_include.lua", "flatbuffers")
    files "../tools/SchemaCompiler/SchemaCompiler.cpp"
    postbuildcommands "{TOUCH} dummy.cpp"
project "FlatbufCompiler"
    kind "StaticLib"
    dependson "SchemaCompiler"
    files "../PGTA/**.fbs"
    files (_ACTION.."/dummy.cpp")
project "*"
