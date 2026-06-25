#pragma once
#include <Polarity.h>

#include "../UIIcons.h";
#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Polarity
{
	class ImGuiEx
	{
	public:
		static bool DrawButtonImage(Ref<SubTexture2D> tex, const std::string& label);
	};
}

