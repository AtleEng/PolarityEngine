#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

namespace Polarity::ECS
{
	template<typename T>
	class View
	{
	public:
		View(const Entity* entities, size_t size)
			: m_Entities(entities), m_Size(size) {}

		const Entity* begin() const { return m_Entities; }
		const Entity* end()   const { return m_Entities + m_Size; }

	private:
		const Entity* m_Entities;
		size_t m_Size;
	};

	class Registry
	{
	public:
		Registry();
		~Registry();

		// EntityManager
		Entity CreateEntity();
		void DestroyEntity(Entity entity);

		int GetEntityAmount();

		// ComponentManager
		template<typename T>
		void RegisterComponent()
		{
			m_ComponentManager.RegisterComponent<T>();
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			return m_ComponentManager.GetComponentType<T>();
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			m_ComponentManager.AddComponent<T>(entity, component);

			auto signature = m_EntityManager.GetSignature(entity);
			signature.set(m_ComponentManager.GetComponentType<T>(), true);
			m_EntityManager.SetSignature(entity, signature);

			m_SystemManager.EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			m_ComponentManager.RemoveComponent<T>(entity);

			auto signature = m_EntityManager.GetSignature(entity);
			signature.set(mComponentManager.GetComponentType<T>(), false);
			m_EntityManager.SetSignature(entity, signature);

			m_SystemManager.EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return m_ComponentManager.GetComponent<T>(entity);
		}

		template<typename T>
		bool HasComponent(Entity entity)
		{
			return m_ComponentManager.HasComponent<T>(entity);
		}

		template<typename T>
		View<T> GetView()
		{
			auto storage = m_ComponentManager.GetComponentArray<T>();
			return View<T>(storage->Entities(), storage->Size());
		}

	private:
		ComponentManager m_ComponentManager;
		EntityManager	 m_EntityManager;
		SystemManager	 m_SystemManager;
	};
}