#include "polpch.h"
#include "Registry.h"

namespace Polarity::ECS
{
	Registry::Registry()
	{
		m_EntityManager = EntityManager();
		m_ComponentManager = ComponentManager();
	}

	Registry::~Registry()
	{
	}

	Entity Registry::CreateEntity()
	{
		return m_EntityManager.CreateEntity();
	}

	void Registry::DestroyEntity(Entity entity)
	{
		m_ComponentManager.EntityDestroyed(entity);

		m_EntityManager.DestroyEntity(entity);

		m_SystemManager.EntityDestroyed(entity);
	}

	bool Registry::IsAlive(Entity entity)
	{
		return entity != INVALID_ENTITY && m_EntityManager.IsAlive(entity);
	}

	int Registry::GetEntityAmount()
	{
		return m_EntityManager.GetEntityAmount();
	}

	void Registry::Clear()
	{
		for (Entity ent : m_EntityManager.m_AliveEntities)
		{
			DestroyEntity(ent);
		}		
	}

	EntityMeta& Registry::GetMeta(Entity entity)
	{
		return m_EntityManager.GetMeta(entity);
	}
}