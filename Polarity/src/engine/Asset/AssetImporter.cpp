#include "polpch.h"
#include "AssetImporter.h"
#include "TextureImporter.h"

#include <map>

namespace Polarity
{
	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		//{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D }
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			POL_CORE_ERROR("No importer available for asset type: %i", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}
}
