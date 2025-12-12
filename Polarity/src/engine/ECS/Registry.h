#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"

namespace Polarity::ECS
{
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
			m_ComponentManager->RegisterComponent<T>();
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			return m_ComponentManager->GetComponentType<T>();
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			m_ComponentManager->AddComponent<T>(entity, component);

			auto signature = m_EntityManager->GetSignature(entity);
			signature.set(m_ComponentManager->GetComponentType<T>(), true);
			m_EntityManager->SetSignature(entity, signature);

			//m_SystemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			m_ComponentManager->RemoveComponent<T>(entity);

			auto signature = m_EntityManager->GetSignature(entity);
			signature.set(mComponentManager->GetComponentType<T>(), false);
			m_EntityManager->SetSignature(entity, signature);

			//m_SystemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return m_ComponentManager->GetComponent<T>(entity);
		}

	private:
		Scope<ComponentManager> m_ComponentManager;
		Scope<EntityManager>	m_EntityManager;
		//Scope<SystemManager> m_SystemManager;
	};
}