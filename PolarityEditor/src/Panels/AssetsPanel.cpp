#include "polpch.h"
#include "AssetsPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void AssetsPanel::OnImGuiRender(EditorContext& ctx)
	{
		ImGui::Begin("Assets", &m_IsOpen);

		ImGui::End();
	}
}