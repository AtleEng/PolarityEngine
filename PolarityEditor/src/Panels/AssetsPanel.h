#pragma once
#include "EditorPanel.h"

#include "engine/utils/PlatformUtils.h"

#include "imgui/imgui.h"
#include <filesystem>

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        static constexpr PanelType StaticPanelID = PanelType::Assets;

        AssetsPanel();

        void OnDraw() override;

    private:
        void DrawHeader();
        void DrawAssets();

		struct TreeNode
		{
			std::filesystem::path Path;
			AssetHandle Handle = 0;

			uint32_t Parent = (uint32_t)-1;
			std::map<std::filesystem::path, uint32_t> Children;

			TreeNode(const std::filesystem::path& path, AssetHandle handle)
				: Path(path), Handle(handle) {
			}
		};
		struct BrowserItem
		{
			std::filesystem::path Path;
			AssetHandle Handle;
			uint32_t TreeNodeIndex;

			bool IsDirectory;
			bool IsAsset;

			Ref<SubTexture2D> Icon;
		};
		void DrawListItem(const BrowserItem& item);
		void DrawIconItem(const BrowserItem& item, float thumbnailSize);

		void DrawList(const std::vector<BrowserItem>& items);
		void DrawIcons(const std::vector<BrowserItem>& items);
        
		std::vector<BrowserItem> BuildBrowserItems();
		TreeNode* GetCurrentTreeNode();
		void RefreshAssetTree();

    private:
        std::filesystem::path m_BaseDirectory;
        std::filesystem::path m_CurrentDirectory;
		uint32_t m_CurrentNode = 0;
		

		std::vector<TreeNode> m_TreeNodes;

		std::map<std::filesystem::path, std::vector<std::filesystem::path>> m_AssetTree;

		enum class View
		{
			List = 0, Icons = 1
		};
		View m_View = View::List;
		enum class Mode
		{
			Asset = 0, FileSystem = 1
		};
		Mode m_Mode = Mode::Asset;
		class BrowserMode
		{
		public:
			virtual ~BrowserMode() = default;

			virtual void OnDoubleClick(const BrowserItem& item) = 0;
			virtual void BeginDrag(const BrowserItem& item) = 0;
			virtual void ContextMenu(const BrowserItem& item) = 0;
		};
		class AssetMode : public BrowserMode
		{
		public:
			virtual void OnDoubleClick(const BrowserItem& item) override
			{

			}
			virtual void BeginDrag(const BrowserItem& item) override
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("ASSET_ITEM", &item.Handle, sizeof(AssetHandle));
					ImGui::EndDragDropSource();
				}
			}
			virtual void ContextMenu(const BrowserItem& item) override
			{
				if (ImGui::BeginPopupContextItem(item.Path.string().c_str()))
				{
					if (ImGui::MenuItem("Open"))
					{
						POL_CORE_ASSERT(false, "Not implemented");
					}
					if (ImGui::MenuItem("Delete"))
					{
						POL_CORE_ASSERT(false, "Not implemented");
					}
					ImGui::EndPopup();
				}
			}
		};
		class FilesystemMode : public BrowserMode
		{
		public:
			virtual void OnDoubleClick(const BrowserItem& item) override
			{

			}
			virtual void BeginDrag(const BrowserItem& item) override
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("PATH_ITEM", &item.Path, sizeof(std::filesystem::path));
					ImGui::EndDragDropSource();
				}
			}
			virtual void ContextMenu(const BrowserItem& item) override
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Import"))
					{
						Project::GetActive()->GetEditorAssetManager()->ImportAsset(item.Path);
					}
					if (ImGui::MenuItem("Open.."))
					{
						FileDialogs::ShowFolder(item.Path);
					}
					ImGui::EndPopup();
				}
			}
		};
		AssetMode m_AssetMode;
		FilesystemMode m_FilesystemMode;
		BrowserMode* m_BrowserMode = &m_AssetMode;
    };
}