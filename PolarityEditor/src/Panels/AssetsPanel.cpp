#include "polpch.h"
#include "AssetsPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void AssetsPanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		uint32_t texID2 = m_Context->Textures[0]->GetTexture()->GetRendererID();
		const glm::vec2* uvs = m_Context->Textures[0]->GetTexCoords();
		ImGui::Image(
			(void*)texID2,
			ImVec2(18, 18),
			ImVec2(uvs[2].x, uvs[0].y),
			ImVec2(uvs[0].x, uvs[2].y)
		);

		ImGui::End();
	}
}