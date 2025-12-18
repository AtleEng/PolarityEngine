#include "polpch.h"
#include "Scene.h"



#include "engine/renderer/Renderer2D.h"
#include "engine/utils/Random.h"

#include "Components.h"
#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Polarity
{
	
	Scene::Scene()
		:m_Registry()
	{
		m_Registry.RegisterComponent<NameComponent>();
		m_Registry.RegisterComponent<TransformComponent>();
		m_Registry.RegisterComponent<SpriteComponent>();
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep tS)
	{
		auto group = m_Registry.GetGroup<TransformComponent, SpriteComponent>();

		for (auto entity : group)
		{
			auto& [transform, sprite] = group.Get(entity);

			Renderer2D::DrawQuad(transform.Transform, sprite.Color);
		}

		for (auto e : m_DestroyQueue)
		{
			m_DestroyQueue.push_back(e);
		}
	}

	Entity Scene::Spawn()
	{
		Entity entity = { m_Registry.CreateEntity(), this };

		entity.AddComponent<NameComponent>();

		TransformComponent& transform = TransformComponent();
		transform.Transform[3].x = Random::Float() * 10;
		transform.Transform[3].y = Random::Float() * 10;
		entity.AddComponent<TransformComponent>(transform);

		entity.AddComponent<SpriteComponent>();

		LOG_DEBUG("Spawning entity: %d", entity.GetComponent<NameComponent>().Name);
		return entity;
	}

	void Scene::Kill(std::string name)
	{
		ECS::Entity target = ECS::INVALID_ENTITY;

		for (auto e : m_Registry.GetView<NameComponent>())
		{
			if (m_Registry.GetComponent<NameComponent>(e).Name == name)
			{
				target = e;
				break;
			}
		}

		if (target != ECS::INVALID_ENTITY)
		{
			LOG_DEBUG("Destroying entity: %d", target);
			m_Registry.DestroyEntity(target);
		}
		else
		{
			LOG_DEBUG("Kill(%s) failed", name.c_str());
		}
	}

	void Scene::List()
	{

		std::string s = "\n Entity List:";
		for (auto entity : m_Registry.GetView<NameComponent>())
		{
			s += "\n  ";
			if (m_Registry.HasComponent<NameComponent>(entity))
			{
				const NameComponent& compName = m_Registry.GetComponent<NameComponent>(entity);
				s += compName.Name;
			}
		}

		LOG_DEBUG(s.c_str());
	}
}