#pragma once

#include <filesystem>

#include "engine/core/Core.h"
#include "engine/core/Log.h"
#include "engine/Asset/AssetsManagerBase.h"
#include "engine/Asset/EditorAssetsManager.h"
#include "engine/Asset/RuntimeAssetsManager.h"

namespace Polarity
{
	struct ProjectConfig
	{
		std::string Name = "Untitled Project";

		std::filesystem::path StartScene; //TODO Make this a id reference

		std::filesystem::path AssetDirectory;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			POL_CORE_ASSERT(s_ActiveProject, "Project: No active project!");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			POL_CORE_ASSERT(s_ActiveProject, "Project: No active project!");
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		// TODO: move to a asset manager
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			POL_CORE_ASSERT(s_ActiveProject, "Project: No active project!");
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }
		Ref<AssetsManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<RuntimeAssetsManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetsManager>(m_AssetManager); }
		Ref<EditorAssetsManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetsManager>(m_AssetManager);}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetsManagerBase> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}
