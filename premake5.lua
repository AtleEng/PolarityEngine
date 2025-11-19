
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
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includedir = {}
includedir["GLFW"]      = "%{wks.location}/Polarity/thirdparty/GLFW/include"
includedir["Glad"]      = "%{wks.location}/Polarity/thirdparty/Glad/include"
includedir["ImGui"]     = "%{wks.location}/Polarity/thirdparty/imgui"
includedir["glm"]       = "%{wks.location}/Polarity/thirdparty/glm"
includedir["stb_image"] = "%{wks.location}/Polarity/thirdparty/stb_image"


group "Dependencies"
    include "Polarity/thirdparty/GLFW"
    include "Polarity/thirdparty/Glad"
    include "Polarity/thirdparty/imgui"
group ""


group "Core"
	include "Polarity"
group ""

group "Misc"
	include "Sandbox"
group ""