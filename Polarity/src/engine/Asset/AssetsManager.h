#pragma once

#include "AssetsManagerBase.h"

#include "engine/Project/Project.h"

namespace Polarity
{
	class AssetsManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle)
		{
			return std::static_pointer_cast<T>(Project::GetActive()->GetAssetManager()->GetAsset());
		}
	};
}