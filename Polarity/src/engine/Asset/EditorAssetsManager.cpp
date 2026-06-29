#include "polpch.h"
#include "EditorAssetsManager.h"

#include "AssetImporter.h"

namespace Polarity
{
    bool EditorAssetsManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
    }
    bool EditorAssetsManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }
    AssetType EditorAssetsManager::GetAssetType(AssetHandle handle) const
    {
        if (!IsAssetHandleValid(handle))
            return AssetType::None;

        return m_AssetRegistry.at(handle).Type;
    }
    void EditorAssetsManager::ImportAsset(const std::filesystem::path& filepath)
    {

    }

    const AssetMetadata& EditorAssetsManager::GetMetadata(AssetHandle handle) const
    {
        static AssetMetadata s_NullMetadata;

        auto it = m_AssetRegistry.find(handle);
        if (it == m_AssetRegistry.end())
            return s_NullMetadata;

        return it->second;
    }

    const std::filesystem::path& EditorAssetsManager::GetFilePath(AssetHandle handle) const
    {
        return GetMetadata(handle).FilePath;
    }

    Ref<Asset> EditorAssetsManager::GetAsset(AssetHandle handle)
    {
        if (!IsAssetHandleValid(handle))
            return nullptr;

        //check if loaded, else load
        Ref<Asset> asset;
        if (IsAssetLoaded(handle))
        {
            asset = m_LoadedAssets.at(handle);
        }
        else
        {
            const AssetMetadata& metadata = GetMetadata(handle);
            asset = AssetImporter::ImportAsset(handle, metadata);
            if (!asset) // Import failed :(
                POL_CORE_ERROR("Asset import failed! %s", metadata.FilePath.string().c_str());

            m_LoadedAssets[handle] = asset;
        }
        return asset;
    }
}
