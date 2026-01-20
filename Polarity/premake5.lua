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

    defines {
        'POLARITY_ARCH="%{cfg.architecture}"',
        'POLARITY_PLATFORM="%{cfg.system}"',
        'POLARITY_CONFIG="%{cfg.buildcfg}"'
    }

    files
    {
        "src/**.h",
        "src/**.cpp",

        "thirdparty/stb_image/**.h",
        "thirdparty/stb_image/**.cpp",

        "thirdparty/glm/glm/**.hpp",
        "thirdparty/glm/glm/**.inl",

        "thirdparty/ImGuizmo/**.h",
        "thirdparty/ImGuizmo/**.cpp"
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
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
        "%{includedir.stb_image}",
        "%{includedir.yaml_cpp}",
        "%{includedir.ImGuizmo}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "yaml-cpp"
    }

    filter "files:thirdparty/ImGuizmo/**.cpp"
    flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "POLARITY_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines
        {
            "POLARITY_DEBUG",
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