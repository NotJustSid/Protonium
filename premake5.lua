workspace("Protonium")
    configurations({"Debug", "Release"})
    language("C++")
    cppdialect("C++17")
    platforms({"x86", "x64"})

project("Protonium")
    kind("ConsoleApp")
    targetname("proto")
    targetdir("bin/Output/%{cfg.buildcfg}-%{cfg.platform}")
    objdir("bin/Intermediates/%{cfg.buildcfg}-%{cfg.platform}")

    files("src/**.cpp", "src/**.hpp")

    filter("configurations:Debug")
        defines({"DEBUG"})
        symbols("On")

    filter("configurations:Release")
        defines({"NDEBUG"})
        optimize("On")
