#include "polpch.h"
#include "GamePreviewPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void GamePreviewPanel::OnDraw()
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open, ImGuiWindowFlags_NoCollapse);

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        ImVec2 availSize = ImGui::GetContentRegionAvail();

        // Get framebuffer / texture size
        uint32_t textureID = m_Context->PreviewFramebuffer->GetColorAttachmentRendererID();
           auto spec = m_Context->PreviewFramebuffer->GetSpecification();
           float texWidth = (float)spec.Width;
        float texHeight = (float)spec.Height;

        float texAspect = texWidth / texHeight;
        float panelAspect = availSize.x / availSize.y;

        ImVec2 imageSize;
        if (panelAspect > texAspect)
        {
            imageSize.y = availSize.y;
            imageSize.x = imageSize.y * texAspect;
        }
        else
        {
            imageSize.x = availSize.x;
            imageSize.y = imageSize.x / texAspect;
        }

        // Center image
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos({
            cursorPos.x + (availSize.x - imageSize.x) * 0.5f,
            cursorPos.y + (availSize.y - imageSize.y) * 0.5f
            });

        ImGui::Image(
            (void*)textureID,
            imageSize,
            ImVec2{ 0, 1 },
            ImVec2{ 1, 0 }
        );

        ImGui::End();
        ImGui::PopStyleVar();
	}
}