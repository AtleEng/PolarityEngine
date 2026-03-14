#pragma once

#include "engine/core/Timestep.h"
#include "engine/ecs/Registry.h"
#include "engine/renderer/EditorCamera.h"
#include "engine/core/UUID.h"

namespace Polarity
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);
		

		Entity CreateEntity(const std::string name = "Entity");
		Entity CreateEntityWithUUID(UUID uuid, const std::string name = "Entity");

		void DestroyEntity(std::string name);
		void DestroyEntity(ECS::Entity handle);
		void ClearEntities();

		bool IsAlive(ECS::Entity handle);
		Entity GetPrimaryCameraEntity();

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

		ECS::Registry& GetRegistry() { return m_Registry; }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity)
		{
			if (entity.HasComponent<CameraComponent>())
			{
				entity.GetComponent<CameraComponent>().Camera.SetViewPortSize(m_ViewportWidth, m_ViewportHeight);
			}
		}
	private:
		ECS::Registry m_Registry;
		std::vector<ECS::Entity> m_DestroyQueue;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
	};
}