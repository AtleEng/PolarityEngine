#include "polpch.h"
#include "ViewportPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void ViewportPanel::OnDraw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open, ImGuiWindowFlags_NoCollapse);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();


		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		glm::vec2 viewportSize = m_Context->ViewportSize;

		glm::vec2 newSize = { viewportPanelSize.x, viewportPanelSize.y };
		if (viewportSize != newSize)
		{
			m_Context->ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		uint32_t textureID = m_Context->ViewportFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(
			(void*)textureID, 
			ImVec2{ viewportSize.x, viewportSize.y },
			{ 0,1 }, 
			{ 1,0 }
		);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}