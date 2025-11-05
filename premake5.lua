workspace "Polarity"
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
includedir["GLFW"] = "Polarity/thirdparty/GLFW/include"
includedir["Glad"] = "Polarity/thirdparty/Glad/include"
includedir["ImGui"] = "Polarity/thirdparty/imgui"
includedir["glm"] = "Polarity/thirdparty/glm"


group "Dependencies"
    include "Polarity/thirdparty/GLFW"
    include "Polarity/thirdparty/Glad"
    include "Polarity/thirdparty/imgui"
group ""

-- Atlas Project (static lib)
project "Polarity"
    location "Polarity"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "polpch.h"
    pchsource "Polarity/src/polpch.cpp"

    files
    {
        "Polarity/src/**.h",
        "Polarity/src/**.cpp",
    }
 
    vpaths
    {
        ["Header Files/*"] = { "Polarity/**.h"},
        ["Source Files/*"] = { "Polarity/**.cpp" }
    }

    includedirs
    {
        "Polarity/src",
        "Polarity/thirdparty",
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
            "POLARITY_PLATFORM_WINDOWS",
            "POLARITY_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "POLARITY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "POLARITY_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "POLARITY_DIST"
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
        "Sandbox/src/**.cpp",
        "polarity.rc"
    }
        
    vpaths
    {
        ["Header Files"] = { "**.h" },
        ["Source Files"] = { "**.cpp" }
    }

    includedirs
    {
        "Polarity/src",
        "Polarity/thirdparty",
        "%{includedir.glm}"
    }

    links
    {
        "Polarity"
    }

    filter "system:windows"
    systemversion "latest"
        defines
        {
            "POLARITY_PLATFORM_WINDOWS",
            "POLARITY_ENABLE_ASSERTS",
        }

    filter "configurations:Debug"
        defines "POLARITY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "POLARITY_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "POLARITY_DIST"
        runtime "Release"
        optimize "on"
