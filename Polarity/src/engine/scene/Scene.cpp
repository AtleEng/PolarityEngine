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

		ECS::Entity e = Spawn();
	}

	Scene::~Scene()
	{

	}
	ECS::Entity Scene::Spawn()
	{
		LOG_DEBUG("Spawning entity: %d", m_Registry.GetEntityAmount() + 1);
		ECS::Entity e = m_Registry.CreateEntity();
		NameComponent n = NameComponent("Name: ");
		m_Registry.AddComponent(e, n);
		return e;
	}
	void Scene::Kill(ECS::Entity e)
	{
		LOG_DEBUG("Killing: %d", m_Registry.GetEntityAmount());

		m_Registry.DestroyEntity(m_Registry.GetEntityAmount());
	}
	void Scene::List()
	{
		std::string s = "Entity List:";
		for (ECS::Entity e = 0; e < m_Registry.GetEntityAmount(); e++)
		{
			s += "\n e: " + std::to_string(e);
			NameComponent t = m_Registry.GetComponent<NameComponent>(e);
			s += "\n   t: " + t.Name;
		}
		LOG_DEBUG(s.c_str());
	}
}