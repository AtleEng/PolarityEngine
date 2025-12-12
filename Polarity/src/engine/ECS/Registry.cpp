#include "polpch.h"
#include "Registry.h"

namespace Polarity::ECS
{

	Registry::Registry()
	{
		m_EntityManager = CreateScope<EntityManager>();
		m_ComponentManager = CreateScope<ComponentManager>();
	}

	Registry::~Registry()
	{
	}

	Entity Registry::CreateEntity()
	{
		return m_EntityManager->CreateEntity();
	}

	void Registry::DestroyEntity(Entity entity)
	{
		m_ComponentManager->EntityDestroyed(entity);

		m_EntityManager->DestroyEntity(entity);
	}

	int Registry::GetEntityAmount()
	{
		return m_EntityManager->GetEntityAmount();
	}
}