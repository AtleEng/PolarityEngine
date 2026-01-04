#pragma once

#include "Scene.h"

namespace Polarity
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		//Serialize scene to text
		bool Serialize(const std::string& filepath);
		//Serialize scene to binary
		bool SerializeRuntime(const std::string& filepath);

		//DeSerialize scene to text
		bool DeSerialize(const std::string& filepath);
		//DeSerialize scene to binary
		bool DeSerializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

