#include "polpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Polarity
{
	Entity::Entity(ECS::Entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	const std::string& Entity::GetName()
	{
		return GetComponent<NameComponent>().Name;
	}

	bool Entity::IsAlive()
	{
		return m_Scene->IsAlive(m_EntityHandle);
	}
	ECS::EntityMeta& Entity::GetMeta()
	{
		return m_Scene->GetRegistry().GetMeta(m_EntityHandle);
	}
}