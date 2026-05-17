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
        "thirdparty/ImGuizmo/**.cpp",

        "thirdparty/imgui/imgui.cpp",
        "thirdparty/imgui/imgui_draw.cpp",
        "thirdparty/imgui/imgui_tables.cpp",
        "thirdparty/imgui/imgui_widgets.cpp",
        "thirdparty/imgui/imgui_demo.cpp"
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
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",

        "%{IncludeDir.VulkanSDK}"
    }

    links
    {
        "GLFW",
        "Glad",
        --"ImGui",
        "opengl32.lib",
        "yaml-cpp"
    }

    filter "files:thirdparty/ImGuizmo/**.cpp"
    flags { "NoPCH" }
    filter "files:thirdparty/ImGui/**.cpp"
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
        links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines
        {
            "POLARITY_RELEASE"
        }
        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        defines
        {
            "POLARITY_DIST"
        }
        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}