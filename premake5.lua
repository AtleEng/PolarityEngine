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

includedir = {}
includedir["GLFW"] = "Atlas/thirdparty/GLFW/include"

include "Atlas/thirdparty/GLFW"

-- Atlas Project (DLL)
project "Atlas"
    location "Atlas"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    systemversion "latest"

    targetdir (path.getabsolute("bin/" .. outputdir .. "/%{prj.name}"))
    objdir (path.getabsolute("bin-int/" .. outputdir .. "/%{prj.name}"))

    pchheader "atlaspch.h"
    pchsource "Atlas/src/atlaspch.cpp"

    files
    {
        "Atlas/src/**.h",
        "Atlas/src/**.cpp"
    }
 
    vpaths
    {
        ["Header Files/*"] = { "Atlas/**.h"},
        ["Source Files/*"] = { "Atlas/**.cpp" }
    }

    includedirs
    {
        "Atlas/src",
        "Atlas/thirdparty",
        "%{includedir.GLFW}"
    }

    links
    {
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
        defines
        {
            "ATLAS_PLATFORM_WINDOWS",
            "ATLAS_BUILD_DLL"
        }
        staticruntime "On"

        -- Copy DLL to Sandbox folder after build
        postbuildcommands
        {
            ("{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        buildoptions "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        buildoptions "/MD"
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
        
    vpaths
    {
        ["Header Files"] = { "**.h" },
        ["Source Files"] = { "**.cpp" }
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
        staticruntime "On"

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        buildoptions "/MD"
        optimize "On"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        buildoptions "/MD"
        optimize "On"
