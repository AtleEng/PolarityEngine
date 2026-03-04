project "spirv-cross-core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "spirv_cross.cpp",
        "spirv_parser.cpp",
        "spirv_cross_parsed_ir.cpp",
        "spirv_cfg.cpp",
        "spirv_cross_util.cpp"
    }

    includedirs
    {
        "thirdparty/shaderc/libshaderc/include"
    }
    links
    {
        "shaderc_combined"
    }

project "spirv-cross-glsl"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "spirv_glsl.cpp"
    }

    includedirs
    {
        "."
    }

    links
    {
        "spirv-cross-core"
    }