#include "polpch.h"
#include "EntityManager.h"


namespace Polarity::ECS
{

	EntityManager::EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			m_AvailableEntities.push(entity);
			m_Alive[entity] = false;
		}
	}

	Entity EntityManager::CreateEntity()
	{
		if (m_AliveEntities.size() >= MAX_ENTITIES)
		{
			POL_CORE_ERROR("ECS: CreateEntity() failed, too many entities in existence %d+!", MAX_ENTITIES);
			return INVALID_ENTITY;
		}

		Entity entity = m_AvailableEntities.front();
		m_AvailableEntities.pop();

		m_Alive[entity] = true;
		m_AliveEntities.push_back(entity);
		m_Meta.push_back({true}); // enabled

		return entity;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			POL_CORE_ERROR("ECS: DestroyEntity() failed, entity out of range id: %d!", entity);
			return;
		}
		if (m_AliveEntities.size() <= 0)
		{
			POL_CORE_ERROR("ECS: DestroyEntity() failed, no living entities left.");
			return;
		}

		m_Signatures[entity].reset();
		m_AvailableEntities.push(entity);
		m_Alive[entity] = false;
		m_Meta[entity] = {};

		auto it = std::find(m_AliveEntities.begin(), m_AliveEntities.end(), entity);
		if (it != m_AliveEntities.end())
		{
			*it = m_AliveEntities.back();
			m_AliveEntities.pop_back();
		}
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			POL_CORE_ERROR("ECS: SetSignature() failed, entity out of range id: %d!", entity);
			return;
		}

		
		m_Signatures[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			POL_CORE_ERROR("ECS: GetSignature() failed, entity out of range id: %d!", entity);
			return Signature{};
		}

		return m_Signatures[entity];
	}

	bool EntityManager::IsAlive(Entity entity)
	{
		return entity < MAX_ENTITIES&& m_Alive[entity];
	}

	int EntityManager::GetEntityAmount()
	{
		return m_AliveEntities.size();
	}

	EntityMeta& EntityManager::GetMeta(Entity entity)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			POL_CORE_ERROR("ECS: GetMeta() failed, entity out of range id: %d!", entity);
			return EntityMeta{};
		}

		return m_Meta[entity];
	}
}
