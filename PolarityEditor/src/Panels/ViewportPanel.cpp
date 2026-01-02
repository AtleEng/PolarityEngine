#include "polpch.h"
#include "ViewportPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void ViewportPanel::OnDraw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport", &m_IsOpen, ImGuiWindowFlags_NoCollapse);
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer().BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		glm::vec2 newSize = { viewportSize.x, viewportSize.y };
		if (m_viewportSize != newSize)
		{
			m_viewportSize = { viewportSize.x, viewportSize.y };
		}
		uint32_t textureID = m_Context->ViewportFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y },
			{ 0,1 }, { 1,0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}
}