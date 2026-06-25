#include "ImGuiEx.h"

namespace Polarity
{
	bool ImGuiEx::DrawButtonImage(Ref<SubTexture2D> tex, const std::string& label)
	{
		bool isKlicked = false;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushID(label.c_str());

		float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 btnSize(lineHeight, lineHeight);

		uint32_t texID = tex->GetTexture()->GetRendererID();
		const glm::vec2* uvs = tex->GetTexCoords();
		if (ImGui::ImageButton((void*)texID,
			btnSize,
			ImVec2(uvs[0].x, uvs[2].y),
			ImVec2(uvs[2].x, uvs[0].y)))
		{
			isKlicked = true;
		}

		ImGui::PopID();
		ImGui::PopStyleVar();
		return isKlicked;
	}
}
