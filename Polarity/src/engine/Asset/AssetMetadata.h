#pragma once

#include <filesystem>

#include "Asset.h"

namespace Polarity 
{
	struct AssetMetadata
	{
		AssetType Type;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};
}
