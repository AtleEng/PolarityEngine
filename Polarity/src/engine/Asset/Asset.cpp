#include "polpch.h"
#include "Asset.h"

namespace Polarity
{
	std::string_view Polarity::AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None:		return "AssetType::None";
		case AssetType::Scene:		return "AssetType::Scene";
		case AssetType::Texture2D:	return "AssetType::Texture2D";
		case AssetType::Audio:		return "AssetType::Audio";
		case AssetType::Script:		return "AssetType::Script";
		}

		return "AssetType::<Invalid>";
	}

	AssetType Polarity::AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "AssetType::None")			return AssetType::None;
		if (assetType == "AssetType::Scene")		return AssetType::Scene;
		if (assetType == "AssetType::Texture2D")	return AssetType::Texture2D;
		if (assetType == "AssetType::Audio")		return AssetType::Audio;
		if (assetType == "AssetType::Script")		return AssetType::Script;

		return AssetType::None;
	}
}
