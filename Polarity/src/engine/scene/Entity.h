#pragma once

#include "Scene.h"
#include "Components.h"

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
			if (HasComponent<T>())
			{
				POL_CORE_WARN("Scene: AddComponent() failed for: %s", GetName().c_str());
				return GetComponent<T>();
			}
			T& newComponent = m_Scene->m_Registry.AddComponent(m_EntityHandle, component);
			m_Scene->OnComponentAdded<T>(*this);
			return newComponent;
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
				POL_CORE_WARN("Scene: RemovedComponent() failed for: %s", GetName().c_str());
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

		const std::string& GetName();
		const uint32_t GetID() { return m_EntityHandle; }
		bool IsAlive();


		operator bool() const { return m_Scene->IsAlive(m_EntityHandle); }

		operator ECS::Entity() const { return m_EntityHandle; }

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }

		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		ECS::Entity m_EntityHandle{ ECS::INVALID_ENTITY };
		Scene* m_Scene = nullptr;
	};
}