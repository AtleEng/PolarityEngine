#include "polpch.h"
#include "AssetsPanel.h"

#include "ImGuiEx.h"




namespace Polarity
{

	AssetsPanel::AssetsPanel()
		: EditorPanel(StaticPanelID, 0, "Assets")
	{
		
		m_BaseDirectory = Project::GetActiveAssetDirectory();
		m_CurrentDirectory = m_BaseDirectory;

		m_TreeNodes.push_back(TreeNode(".", 0));
		RefreshAssetTree();
	}

	void AssetsPanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);
		
		DrawHeader();
		DrawAssets();
		
		ImGui::End();
	}
	
	void AssetsPanel::DrawHeader()
	{

		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::ArrowBack), "##Back"))
		{
			if (m_Mode == Mode::Asset)
			{
				if (m_CurrentNode != 0)
				{
					m_CurrentNode = m_TreeNodes[m_CurrentNode].Parent;
					m_CurrentDirectory = m_TreeNodes[m_CurrentNode].Path;
				}
			}
			else
			{
				if (m_CurrentDirectory != m_BaseDirectory)
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		ImGui::SameLine();
		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Refresh), "##Refresh"))
		{
			RefreshAssetTree();
		}
		ImGui::SameLine();
		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Folder), "##OpenFolder"))
		{
			FileDialogs::ShowFolder(m_CurrentDirectory);
		}
		ImGui::SameLine();
		ImGui::Text(m_CurrentDirectory.string().c_str());

		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::ViewList), "##View"))
		{
			if (m_View == View::List)
			{
				m_View = View::Icons;
			}
			else
			{
				m_View = View::List;
			}
		}
		ImGui::SameLine();
		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Select), "##Mode"))
		{
			if (m_Mode == Mode::Asset)
			{
				m_Mode = Mode::FileSystem;
				m_CurrentDirectory = m_BaseDirectory;
				m_BrowserMode = &m_FilesystemMode;
			}
			else
			{
				m_Mode = Mode::Asset;
				m_CurrentNode = 0;
				m_CurrentDirectory = m_BaseDirectory;
				m_BrowserMode = &m_AssetMode;
			}
		}
		ImGui::SameLine();
		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Filter), "##Filter"))
		{
			//Popup with a filter select
		}
		ImGui::SameLine();
		ImGui::Text(m_BaseDirectory.string().c_str());
		ImGui::Separator();
	}

	void AssetsPanel::DrawAssets()
	{
		auto items = BuildBrowserItems();

		switch (m_View)
		{
		case View::List:
			DrawList(items);
			break;

		case View::Icons:
			DrawIcons(items);
			break;
		}
	}
	void AssetsPanel::DrawListItem(const BrowserItem& item)
	{
		ImGui::PushID(item.Path.string().c_str());
		uint32_t texID = item.Icon->GetTexture()->GetRendererID();
		const glm::vec2* uvs = item.Icon->GetTexCoords();

		float rowHeight = 22.0f;
		float fullWidth = ImGui::GetContentRegionAvail().x;

		ImGui::Button("##row", { fullWidth, rowHeight });

		m_BrowserMode->BeginDrag(item);
		m_BrowserMode->ContextMenu(item);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (item.IsDirectory)
			{
				m_CurrentNode = item.TreeNodeIndex;
				m_CurrentDirectory /= item.Path.filename();
			}
			else
			{
				m_BrowserMode->OnDoubleClick(item);
			}
		}

		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();

		ImGui::SetCursorScreenPos({ min.x + 2, min.y + 2 });
		ImGui::Image((void*)texID, { 18,18 }, ImVec2(uvs[0].x, uvs[2].y), ImVec2(uvs[2].x, uvs[0].y));

		ImGui::SetCursorScreenPos({ min.x + 24, min.y + 3 });
		ImGui::TextUnformatted(item.Path.filename().string().c_str());
		ImGui::PopID();
	}

	void AssetsPanel::DrawIconItem(const BrowserItem& item, float thumbnailSize)
	{
		ImGui::PushID(item.Path.string().c_str());
		uint32_t texID = item.Icon->GetTexture()->GetRendererID();
		const glm::vec2* uvs = item.Icon->GetTexCoords();

		ImGui::ImageButton((ImTextureID)texID, { thumbnailSize, thumbnailSize }, ImVec2(uvs[0].x, uvs[2].y), ImVec2(uvs[2].x, uvs[0].y));
		m_BrowserMode->BeginDrag(item);
		m_BrowserMode->ContextMenu(item);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (item.IsDirectory)
			{
				m_CurrentNode = item.TreeNodeIndex;
				m_CurrentDirectory /= item.Path.filename();
			}
			else
			{
				m_BrowserMode->OnDoubleClick(item);
			}
		}
		ImGui::TextWrapped(item.Path.filename().string().c_str());
		ImGui::PopID();
	}

	void AssetsPanel::DrawList(const std::vector<BrowserItem>& items)
	{
		for (const auto& item : items)
		{
			DrawListItem(item);
		}
	}

	void AssetsPanel::DrawIcons(const std::vector<BrowserItem>& items)
	{
		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (const auto& item : items)
		{
			DrawIconItem(item, thumbnailSize);
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);
	}

	std::vector<AssetsPanel::BrowserItem> AssetsPanel::BuildBrowserItems()
	{
		std::vector<BrowserItem> items;

		if (m_Mode == Mode::Asset)
		{
			TreeNode* node = GetCurrentTreeNode();

			for (const auto& [name, index] : node->Children)
			{
				const auto& treeNode = m_TreeNodes[index];

				BrowserItem item;
				item.Path = Project::GetActiveAssetDirectory() / name;
				item.Handle = treeNode.Handle;
				item.TreeNodeIndex = index;
				item.IsDirectory = std::filesystem::is_directory(item.Path);
				
				UIIcon uiIcon = UIIcon::File;
				if (item.IsDirectory)
				{
					uiIcon = UIIcon::Folder;
				}
				else if (item.Path.extension() == ".wav")
				{
					uiIcon = UIIcon::Music;
				}
				else if (item.Path.extension() == ".png")
				{
					uiIcon = UIIcon::Image;
				}
				else if (item.Path.extension() == ".ttf")
				{
					uiIcon = UIIcon::Font;
				}
				else if (item.Path.extension() == ".pol")
				{
					uiIcon = UIIcon::Scene;
				}
				item.Icon = UIIcons::Get(uiIcon);
				
				items.push_back(std::move(item));
			}
		}
		else
		{
			for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				BrowserItem item;

				item.Path = entry.path();
				item.IsDirectory = entry.is_directory();

				UIIcon uiIcon = UIIcon::File;
				if (item.IsDirectory)
				{
					uiIcon = UIIcon::Folder;
				}
				else if (item.Path.extension() == ".wav")
				{
					uiIcon = UIIcon::Music;
				}
				else if (item.Path.extension() == ".png")
				{
					uiIcon = UIIcon::Image;
				}
				else if (item.Path.extension() == ".ttf")
				{
					uiIcon = UIIcon::Font;
				}
				else if (item.Path.extension() == ".pol")
				{
					uiIcon = UIIcon::Scene;
				}
				item.Icon = UIIcons::Get(uiIcon);

				items.push_back(std::move(item));
			}
		}
		return items;
	}

	AssetsPanel::TreeNode* AssetsPanel::GetCurrentTreeNode()
	{
		return &m_TreeNodes[m_CurrentNode];
	}

	void AssetsPanel::RefreshAssetTree()
	{
		m_TreeNodes.clear();

		m_TreeNodes.emplace_back(".", 0);
		m_TreeNodes[0].Parent = 0;

		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [handle, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;

			std::filesystem::path relative =
				std::filesystem::relative(
					metadata.FilePath,
					Project::GetActiveAssetDirectory());

			for (const auto& p : relative)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(p.generic_string());
				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
				}
				else
				{
					// + node
					TreeNode newNode(p, handle);
					newNode.Parent = currentNodeIndex;
					m_TreeNodes.push_back(newNode);

					m_TreeNodes[currentNodeIndex].Children[p] = m_TreeNodes.size() - 1;
					currentNodeIndex = m_TreeNodes.size() - 1;
				}
			}
		}
	}
}