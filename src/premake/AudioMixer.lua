
project "AudioMixer"
    kind "StaticLib"
    includedirs "../AudioMixer/include/"
    files
    {
        "../AudioMixer/**.h",
        "../AudioMixer/**.cpp"
    }
project "*"
