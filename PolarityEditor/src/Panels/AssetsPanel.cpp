#include "polpch.h"
#include "AssetsPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	//temporary
	const static std::filesystem::path s_AssetsPath = "assets";

	AssetsPanel::AssetsPanel()
		: EditorPanel(StaticPanelID, 0, "Assets")
	{
		m_CurrentDirectory = s_AssetsPath;
	}

	void AssetsPanel::OnDraw()
	{
		auto& folderIcon = UIIcons::Get(UIIcon::Folder);
		auto& fileIcon = UIIcons::Get(UIIcon::File);
		auto& musicIcon = UIIcons::Get(UIIcon::Music);

		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);


		if (ImGui::Button("<"))
		{
			if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}		
		}
		ImGui::SameLine();
		ImGui::Text(m_CurrentDirectory.string().c_str());
		ImGui::Separator();

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();

			ImGui::PushID(path.filename().string().c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			Ref<SubTexture2D> icon = fileIcon;
			if (directoryEntry.is_directory())
			{
				icon = folderIcon;
			}
			else if (directoryEntry.path().extension() == ".wav")
			{
				icon = musicIcon;
			}
			uint32_t texID = icon->GetTexture()->GetRendererID();
			const glm::vec2* uvs = icon->GetTexCoords();

			float rowHeight = 22.0f;
			float fullWidth = ImGui::GetContentRegionAvail().x;

			ImGui::Button("##row", { fullWidth, rowHeight });
			ImGui::PopStyleColor();
			// Double click handling
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			// Drag drop
			if (!directoryEntry.is_directory() && ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("ASSETS_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			// Draw contents on top of the button
			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			ImGui::SetCursorScreenPos({ min.x + 2, min.y + 2 });
			ImGui::Image((void*)texID,
				{ 18,18 },
				ImVec2(uvs[0].x, uvs[2].y),
				ImVec2(uvs[2].x, uvs[0].y));

			ImGui::SetCursorScreenPos({ min.x + 24, min.y + 3 });
			ImGui::TextUnformatted(path.filename().string().c_str());



			ImGui::PopID();
		}

		ImGui::End();
	}
}