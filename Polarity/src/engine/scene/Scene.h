#pragma once

#include "engine/ECS/Registry.h"

namespace Polarity
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		ECS::Entity Spawn();
		void Kill(ECS::Entity);
		void List();
	private:
		ECS::Registry m_Registry;
	};
}