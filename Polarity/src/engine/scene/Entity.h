#pragma once

#include "Components.h"
#include "engine/ECS/Registry.h"

namespace Polarity
{
	class Scene;


	class Entity
	{
	public:
		Entity() = default;
		Entity(ECS::Entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		T& AddComponent(T component = T())
		{
			return m_Scene->m_Registry.AddComponent(m_EntityHandle, component);
		}

		template<typename T>
		void RemoveComponent()
		{
			if (HasComponent<T>())
			{
				m_Scene->m_Registry.RemoveComponent<T>(m_EntityHandle);
			}
			else
			{
				LOG_WARN("RemovedComponent() failed for: %s", GetName().c_str());
			}
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Scene->m_Registry.GetComponent<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.HasComponent<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != ECS::INVALID_ENTITY; }
		operator ECS::Entity() const { return m_EntityHandle; }

		const std::string& GetName() { return GetComponent<NameComponent>().Name; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		ECS::Entity m_EntityHandle{ ECS::INVALID_ENTITY };
		Scene* m_Scene = nullptr;
	};
}