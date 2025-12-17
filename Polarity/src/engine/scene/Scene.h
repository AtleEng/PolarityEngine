#pragma once

#include "engine/core/Application.h"
#include "engine/ECS/Registry.h"


namespace Polarity
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep tS);

		ECS::Entity Spawn();
		void Kill(std::string name);
		void List();
	private:
		ECS::Registry m_Registry;
		std::vector<ECS::Entity> m_DestroyQueue;
	};
}