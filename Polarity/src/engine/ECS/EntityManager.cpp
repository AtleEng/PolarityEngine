#include "polpch.h"
#include "EntityManager.h"

namespace Polarity::ECS
{

	EntityManager::EntityManager()
	{
		// Init with IDs
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			m_AvailableEntities.push(entity);
		}
	}

	Entity EntityManager::CreateEntity()
	{
		if (m_LivingEntityCount >= MAX_ENTITIES)
		{
			LOG_ERROR("CreateEntity() failed, too many entities in existence (%d).", MAX_ENTITIES);
			return -1;
		}

		// Take an ID from the front of the queue
		Entity id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_LivingEntityCount;

		return id;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			LOG_ERROR("DestroyEntity() failed, entity out of range. (%d).", entity);
			return;
		}
		// Ensure we cannot go negative
		if (m_LivingEntityCount <= 0)
		{
			LOG_ERROR("DestroyEntity() failed, no living entities left.");
			return;
		}

		// Invalidate the destroyed entity's signature
		m_Signatures[entity].reset();

		// Put the destroyed ID at the back of the queue
		m_AvailableEntities.push(entity);
		--m_LivingEntityCount;
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			LOG_ERROR("SetSignature() failed, entity out of range. (%d).", entity);
			return;
		}

		// Put this entity's signature into the array
		m_Signatures[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		if (entity >= MAX_ENTITIES || entity < 0)
		{
			LOG_ERROR("GetSignature() failed, entity out of range. (%d).", entity);
			return Signature{};
		}

		// Get this entity's signature from the array
		return m_Signatures[entity];
	}

	int EntityManager::GetEntityAmount()
	{
		return m_LivingEntityCount;
	}
}
