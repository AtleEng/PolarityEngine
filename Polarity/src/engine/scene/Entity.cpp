#include "polpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Polarity
{
	Entity::Entity(ECS::Entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
	bool Entity::IsAlive()
	{
		return m_EntityHandle != ECS::INVALID_ENTITY && m_Scene->IsAlive(m_EntityHandle);
	}
}