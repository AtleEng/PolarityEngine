#include "polpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Polarity
{
	Entity::Entity(ECS::Entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}