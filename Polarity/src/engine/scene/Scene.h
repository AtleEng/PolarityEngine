#pragma once

#include "engine/core/Application.h"
#include "engine/ECS/Registry.h"


namespace Polarity
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep tS);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity Spawn(std::string name = "NewEntity");
		void Kill(std::string name);
		bool IsAlive(ECS::Entity handle);
		void List();

		template<typename First, typename... Rest>
		std::vector<Entity> GetView()
		{
			std::vector<Entity> entities;
			for (auto e : m_Registry.GetView<First, Rest...>())
			{
				Entity entity = Entity( e, this );
				entities.push_back(entity);
			}
			return entities;
		}
	private:
		ECS::Registry m_Registry;
		std::vector<ECS::Entity> m_DestroyQueue;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
	};
}