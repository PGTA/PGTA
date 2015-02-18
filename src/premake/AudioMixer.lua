
project "AudioMixer"
    kind "StaticLib"
    includedirs "../AudioMixer/include/"
    --defines "PGTA_EXPORTS"
    --dependson "FlatbufCompiler"
    files
    {
        "../AudioMixer/**.h",
        "../AudioMixer/**.cpp"
    }
project "*"
