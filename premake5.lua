workspace "Atlas"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

-- Output directory format
outputdir = "%{cfg.buildcfg}-%{cfg.system}-x64"

-- Atlas Project (DLL)
project "Atlas"
    location "Atlas"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"

    targetdir (path.getabsolute("bin/" .. outputdir .. "/%{prj.name}"))
    objdir (path.getabsolute("bin-int/" .. outputdir .. "/%{prj.name}"))

    files
    {
        "Atlas/src/**.h",
        "Atlas/src/**.cpp"
    }

    includedirs
    {
        "Atlas/src",
        "Atlas/thirdparty"
    }

    filter "system:windows"
        defines
        {
            "ATLAS_PLATFORM_WINDOWS",
            "ATLAS_BUILD_DLL"
        }
        staticruntime "Off"

        -- Copy DLL to Sandbox folder after build
        postbuildcommands
        {
            ("{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        optimize "On"

-- Sandbox Project (App)
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"

    targetdir (path.getabsolute("bin/" .. outputdir .. "/%{prj.name}"))
    objdir (path.getabsolute("bin-int/" .. outputdir .. "/%{prj.name}"))

    files
    {
        "Sandbox/src/**.h",
        "Sandbox/src/**.cpp"
    }

    includedirs
    {
        "Atlas/src",
        "Atlas/thirdparty"
    }

    links
    {
        "Atlas"
    }

    filter "system:windows"
        defines
        {
            "ATLAS_PLATFORM_WINDOWS"
        }
        staticruntime "Off"

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        optimize "On"
