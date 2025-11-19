-- Engine (static lib)
project "Polarity"
    location "%{wks.location}/Polarity"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "polpch.h"
    pchsource "src/polpch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "thirdparty/stb_image/**.cpp",
        "thirdparty/stb_image/**.h"
    }
 
    vpaths
    {
        ["Header Files/*"] = { "**.h"},
        ["Source Files/*"] = { "**.cpp" }
    }

    includedirs
    {
        "src",
        "thirdparty",
        "%{includedir.GLFW}",
        "%{includedir.Glad}",
        "%{includedir.ImGui}",
        "%{includedir.glm}",
        "%{includedir.stb_image}"
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
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "POLARITY_DEBUG"
        runtime "Debug"
        symbols "on"
        defines
        {
            "POLARITY_ENABLE_ASSERTS"
        }

    filter "configurations:Release"
        defines "POLARITY_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "POLARITY_DIST"
        runtime "Release"
        optimize "on"