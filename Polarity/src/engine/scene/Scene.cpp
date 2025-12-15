#include "polpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

namespace Polarity
{
	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const std::string name)
			: Name(name) {}
	};
	struct TransformComponent
	{
		glm::mat4 Transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
	};

	Scene::Scene()
		:m_Registry()
	{
		m_Registry.RegisterComponent<NameComponent>();
		m_Registry.RegisterComponent<TransformComponent>();
	}

	Scene::~Scene()
	{

	}

	ECS::Entity Scene::Spawn()
	{
		ECS::Entity e = m_Registry.CreateEntity();
		LOG_DEBUG("Spawning entity: %d", e);

		std::string s = "TestComponent to entity: " + std::to_string(e);
		const NameComponent& n = NameComponent(s);
		m_Registry.AddComponent(e, n);
		return e;
	}

	void Scene::Kill(ECS::Entity e)
	{
		LOG_DEBUG("Killing: %d", e);

		m_Registry.DestroyEntity(e);
	}

	void Scene::List()
	{

		std::string s = "\n Entity List:";
		for (auto entity : m_Registry.GetView<NameComponent>())
		{
			s += "\n  entity: ";
			if (m_Registry.HasComponent<NameComponent>(entity))
			{
				const NameComponent& compName = m_Registry.GetComponent<NameComponent>(entity);
				s += "\n    component: " + compName.Name;
			}
		}

		LOG_DEBUG(s.c_str());
	}
}