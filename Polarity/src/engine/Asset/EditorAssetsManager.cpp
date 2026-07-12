#include "polpch.h"
#include "EditorAssetsManager.h"

#include "AssetImporter.h"
#include "engine/Project/Project.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Polarity
{
    static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
        { ".hazel", AssetType::Scene },
        { ".png", AssetType::Texture2D },
        { ".jpg", AssetType::Texture2D },
        { ".jpeg", AssetType::Texture2D }
    };
    static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
    {
        if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
        {
            POL_CORE_WARN("Could not find AssetType for %s", extension.c_str());
            return AssetType::None;
        }

        return s_AssetExtensionMap.at(extension);
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
    {
        out << std::string(v.data(), v.size());
        return out;
    }

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
        AssetHandle handle; // new handle
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = GetAssetTypeFromFileExtension(filepath.extension());
        POL_CORE_ASSERT(metadata.Type != AssetType::None, "%s has no valid assetType", filepath.filename().c_str());
        Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
        if (asset)
        {
            asset->Handle = handle;
            m_LoadedAssets[handle] = asset;
            m_AssetRegistry[handle] = metadata;
            SerializeAssetRegistry();
        }
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

    void EditorAssetsManager::SerializeAssetRegistry()
    {
        auto path = Project::GetActiveAssetRegistryPath();

        YAML::Emitter out;
        {
            out << YAML::BeginMap; // Root
            out << YAML::Key << "AssetRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto& [handle, metadata] : m_AssetRegistry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Handle" << YAML::Value << handle;
                std::string filepathStr = metadata.FilePath.generic_string();
                out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
                out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            out << YAML::EndMap; // Root
        }

        std::ofstream fout(path);
        fout << out.c_str();
    }

    bool EditorAssetsManager::DeserializeAssetRegistry()
    {
        auto path = Project::GetActiveAssetRegistryPath();
        YAML::Node data;
        try
        {
            POL_DEBUG("path: %s", path.string().c_str());
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            POL_CORE_ERROR("Failed to load project file '%s'", path.c_str());
            return false;
        }

        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto& node : rootNode)
        {
            AssetHandle handle = node["Handle"].as<uint64_t>();
            auto& metadata = m_AssetRegistry[handle];
            metadata.FilePath = node["FilePath"].as<std::string>();
            metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
        }

        return true;
    }
}
