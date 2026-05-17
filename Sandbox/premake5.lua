local PolRootDir = '../..'
-- Game DLL (shared lib)
project "Sandbox"
    location "%{wks.location}/Sandbox"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("Binaries")
	objdir ("Intermediates")

    files
    {
        "src/**.h",
        "src/**.cpp",
        "assets/scripts/**.h",
        "assets/scripts/**.cpp"
    }
    vpaths
    {
        ["Header Files"] = { "**.h" },
        ["Source Files"] = { "**.cpp" }
    }

    includedirs
    {
        "%{wks.location}/Polarity/src",
        "%{wks.location}/Polarity/thirdparty",
        "%{IncludeDir.glm}"
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
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"