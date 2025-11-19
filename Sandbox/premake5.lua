-- Sandbox Project (App)
project "Sandbox"
    location "%{wks.location}/Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
        "../polarity.rc"
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