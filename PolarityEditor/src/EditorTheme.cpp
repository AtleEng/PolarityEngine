#include "EditorTheme.h"



#include <fstream>
#include <sstream>

namespace Polarity
{
	std::unordered_map<int, ImVec4> EditorTheme::s_Colors;

	static std::unordered_map<std::string, ImGuiCol> s_ColorMap =
	{
         {"Text", ImGuiCol_Text},
    {"TextDisabled", ImGuiCol_TextDisabled},

    {"WindowBg", ImGuiCol_WindowBg},
    {"ChildBg", ImGuiCol_ChildBg},
    {"PopupBg", ImGuiCol_PopupBg},

    {"Border", ImGuiCol_Border},
    {"BorderShadow", ImGuiCol_BorderShadow},

    {"FrameBg", ImGuiCol_FrameBg},
    {"FrameBgHovered", ImGuiCol_FrameBgHovered},
    {"FrameBgActive", ImGuiCol_FrameBgActive},

    {"TitleBg", ImGuiCol_TitleBg},
    {"TitleBgActive", ImGuiCol_TitleBgActive},
    {"TitleBgCollapsed", ImGuiCol_TitleBgCollapsed},

    {"MenuBarBg", ImGuiCol_MenuBarBg},

    {"ScrollbarBg", ImGuiCol_ScrollbarBg},
    {"ScrollbarGrab", ImGuiCol_ScrollbarGrab},
    {"ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered},
    {"ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive},

    {"CheckMark", ImGuiCol_CheckMark},

    {"SliderGrab", ImGuiCol_SliderGrab},
    {"SliderGrabActive", ImGuiCol_SliderGrabActive},

    {"Button", ImGuiCol_Button},
    {"ButtonHovered", ImGuiCol_ButtonHovered},
    {"ButtonActive", ImGuiCol_ButtonActive},

    {"Header", ImGuiCol_Header},
    {"HeaderHovered", ImGuiCol_HeaderHovered},
    {"HeaderActive", ImGuiCol_HeaderActive},

    {"Separator", ImGuiCol_Separator},
    {"SeparatorHovered", ImGuiCol_SeparatorHovered},
    {"SeparatorActive", ImGuiCol_SeparatorActive},

    {"ResizeGrip", ImGuiCol_ResizeGrip},
    {"ResizeGripHovered", ImGuiCol_ResizeGripHovered},
    {"ResizeGripActive", ImGuiCol_ResizeGripActive},

    {"Tab", ImGuiCol_Tab},
    {"TabHovered", ImGuiCol_TabHovered},
    {"TabActive", ImGuiCol_TabActive},
    {"TabUnfocused", ImGuiCol_TabUnfocused},
    {"TabUnfocusedActive", ImGuiCol_TabUnfocusedActive},

    {"DockingPreview", ImGuiCol_DockingPreview},
    {"DockingEmptyBg", ImGuiCol_DockingEmptyBg},

    {"PlotLines", ImGuiCol_PlotLines},
    {"PlotLinesHovered", ImGuiCol_PlotLinesHovered},
    {"PlotHistogram", ImGuiCol_PlotHistogram},
    {"PlotHistogramHovered", ImGuiCol_PlotHistogramHovered},

    {"TableHeaderBg", ImGuiCol_TableHeaderBg},
    {"TableBorderStrong", ImGuiCol_TableBorderStrong},
    {"TableBorderLight", ImGuiCol_TableBorderLight},
    {"TableRowBg", ImGuiCol_TableRowBg},
    {"TableRowBgAlt", ImGuiCol_TableRowBgAlt},

    {"TextSelectedBg", ImGuiCol_TextSelectedBg},
    {"DragDropTarget", ImGuiCol_DragDropTarget},

    {"NavHighlight", ImGuiCol_NavHighlight},
    {"NavWindowingHighlight", ImGuiCol_NavWindowingHighlight},
    {"NavWindowingDimBg", ImGuiCol_NavWindowingDimBg},
    {"ModalWindowDimBg", ImGuiCol_ModalWindowDimBg}
	};

	bool EditorTheme::Load(const std::filesystem::path& filepath)
	{

		std::ifstream file(filepath);
        if (!file.is_open())
        {
            POL_CORE_ERROR("Couldn't load theme: %s", filepath.string().c_str());
         
			return false;
        }

		s_Colors.clear();
		std::string line;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::stringstream ss(line);

            std::string name;
            float r, g, b, a;

            ss >> name >> r >> g >> b >> a;

            if (auto it = s_ColorMap.find(name); it != s_ColorMap.end())
                s_Colors[it->second] = ImVec4(r, g, b, a);
        }

		return true;
	}

	void EditorTheme::Apply()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;

		for (auto& [id, color] : s_Colors)
			colors[id] = color;
	}
}