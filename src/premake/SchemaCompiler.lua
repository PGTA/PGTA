
filter "files:**.fbs"
    buildmessage "flatc: Compiling %{file.relpath}"
    buildcommands
    {
        'flatc -c -o "../../PGTA/public/schema" %{file.relpath}',
        'SchemaCompiler %{file.relpath}'
    }
    buildoutputs ""
filter {}

project "SchemaCompiler"
    kind "ConsoleApp"
    dependson "flatc"
    files "../tools/SchemaCompiler/SchemaCompiler.cpp"
project "FlatbufCompiler"
    kind "SharedLib"
    dependson "SchemaCompiler"
    files "../PGTA/**.fbs"
project "*"
