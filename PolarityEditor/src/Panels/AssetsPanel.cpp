#include "polpch.h"
#include "AssetsPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void AssetsPanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		ImGui::End();
	}
}