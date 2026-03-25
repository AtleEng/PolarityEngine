
dofile("Dependencies.lua")

workspace "Polarity"
    architecture "x64"
    startproject "PolarityEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    flags
	{
		"MultiProcessorCompile"
	}

-- Output directory format
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
    include "Polarity/thirdparty/GLFW"
    include "Polarity/thirdparty/Glad"
    include "Polarity/thirdparty/imgui"
    include "Polarity/thirdparty/yaml-cpp"

    include "Polarity/thirdparty/SPIRV-Cross"
group ""


group "Core"
	include "Polarity"
group ""

group "Tools"
	include "PolarityEditor"
group ""

group "Misc"
	include "Sandbox"
group ""