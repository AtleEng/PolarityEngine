
-- Polarity Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
assert(VULKAN_SDK, "VULKAN_SDK environment variable not set!")

IncludeDir = {}
IncludeDir["stb_image"] =   "%{wks.location}/Polarity/thirdparty/stb_image"
IncludeDir["yaml_cpp"] =    "%{wks.location}/Polarity/thirdparty/yaml-cpp/include"
IncludeDir["GLFW"] =        "%{wks.location}/Polarity/thirdparty/GLFW/include"
IncludeDir["Glad"] =        "%{wks.location}/Polarity/thirdparty/Glad/include"
IncludeDir["ImGui"] =       "%{wks.location}/Polarity/thirdparty/ImGui"
IncludeDir["ImGuizmo"] =    "%{wks.location}/Polarity/thirdparty/ImGuizmo"
IncludeDir["glm"] =         "%{wks.location}/Polarity/thirdparty/glm"
IncludeDir["shaderc"] =     "%{wks.location}/Polarity/thirdparty/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Polarity/thirdparty/SPIRV-Cross"
IncludeDir["VulkanSDK"] =   "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] =	"%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"