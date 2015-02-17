
filter "files:**.fbs"
    buildmessage "flatc: Compiling %{file.relpath}"
    buildcommands
    {
        'flatc -c -o "../../PGTA/public/schema" %{file.relpath}',
        'SchemaCompiler %{file.relpath}'
    }
    buildoutputs ""
filter {}

local function TOUCH(file)
    if(os.get() == "windows") then
        return "type nul >> "..file.." && copy /b "..file.."+,, "..file
    else
        return "touch "..file
    end
end

project "SchemaCompiler"
    kind "ConsoleApp"
    dependson "flatc"
    files "../tools/SchemaCompiler/SchemaCompiler.cpp"
    postbuildcommands(TOUCH("dummy.cpp"))
project "FlatbufCompiler"
    kind "StaticLib"
    dependson "SchemaCompiler"
    files "../PGTA/**.fbs"
    files (_ACTION.."/dummy.cpp")
project "*"
