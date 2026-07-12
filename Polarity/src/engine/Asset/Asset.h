#pragma once

#include "engine/core/UUID.h"

namespace Polarity
{
	using AssetHandle = UUID;

	enum class AssetType
	{
		None = 0,
		Scene,
		Texture2D,
		Audio,
		Script
	};
	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);

	class Asset
	{
	public:
		virtual AssetType GetType() const = 0;

		AssetHandle Handle;
	};
}

