#include "polpch.h"
#include "AssetsPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void AssetsPanel::OnDraw()
	{
		ImGui::Begin("Assets", &m_IsOpen);

		ImGui::End();
	}
}