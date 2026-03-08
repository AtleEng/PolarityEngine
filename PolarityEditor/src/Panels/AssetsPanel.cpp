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

		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);


		if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))
		{
			if (ImGui::Button("<"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();

			ImGui::PushID(path.filename().string().c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			Ref<SubTexture2D> icon = directoryEntry.is_directory() ? folderIcon : fileIcon;
			uint32_t texID = icon->GetTexture()->GetRendererID();
			const glm::vec2* uvs = icon->GetTexCoords();

			ImGui::ImageButton((void*)texID,
				{ thumbnailSize, thumbnailSize },
				ImVec2(uvs[0].x, uvs[2].y),
				ImVec2(uvs[2].x, uvs[0].y));

			if (!directoryEntry.is_directory() && ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("ASSETS_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(path.filename().string().c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}
}