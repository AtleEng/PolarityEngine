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
includedir["Glad"] = "Atlas/thirdparty/Glad/include"
includedir["ImGui"] = "Atlas/thirdparty/imgui"
includedir["glm"] = "Atlas/thirdparty/glm"


group "Dependencies"
    include "Atlas/thirdparty/GLFW"
    include "Atlas/thirdparty/Glad"
    include "Atlas/thirdparty/imgui"
group ""

-- Atlas Project (static lib)
project "Atlas"
    location "Atlas"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

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
        "%{includedir.GLFW}",
        "%{includedir.Glad}",
        "%{includedir.ImGui}",
        "%{includedir.glm}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "ATLAS_PLATFORM_WINDOWS",
            "ATLAS_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        runtime "Release"
        optimize "on"



-- Sandbox Project (App)
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

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
        "Atlas/thirdparty",
        "%{includedir.glm}"
    }

    links
    {
        "Atlas"
    }

    filter "system:windows"
    systemversion "latest"
        defines
        {
            "ATLAS_PLATFORM_WINDOWS",
            "ATLAS_ENABLE_ASSERTS",
        }

    filter "configurations:Debug"
        defines "ATLAS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ATLAS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ATLAS_DIST"
        runtime "Release"
        optimize "on"
