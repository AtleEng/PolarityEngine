#pragma once
#include <Polarity.h>

#include "imgui/imgui.h"
#include <string>
#include <unordered_map>
#include <filesystem>

namespace Polarity
{
	class EditorTheme
	{
	public:
		static bool Load(const std::filesystem::path& filepath);
		static void Apply();

	private:
		static std::unordered_map<int, ImVec4> s_Colors;
	};
}