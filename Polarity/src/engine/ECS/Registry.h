#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

namespace Polarity::ECS
{
	class Registry;

	template<typename... Components>
	class View
	{
	public:
		View(Registry* registry, const Entity* entities, size_t size)
			: m_Registry(registry), m_Entities(entities), m_Size(size) {}

		class Iterator
		{
		public:
			Iterator(Registry* registry, const Entity* ptr, const Entity* end)
				: m_Registry(registry), m_Ptr(ptr), m_End(end)
			{
				SkipInvalid();
			}

			Entity operator*() const { return *m_Ptr; }

			Iterator& operator++()
			{
				++m_Ptr;
				SkipInvalid();
				return *this;
			}

			bool operator!=(const Iterator& other) const
			{
				return m_Ptr != other.m_Ptr;
			}

		private:
			void SkipInvalid()
			{
				while (m_Ptr != m_End && !(m_Registry->HasComponent<Components>(*m_Ptr) && ...))
					++m_Ptr;
			}

			Registry* m_Registry;
			const Entity* m_Ptr;
			const Entity* m_End;
		};

		Iterator begin() const { return Iterator(m_Registry, m_Entities, m_Entities + m_Size); }
		Iterator end()   const { return Iterator(m_Registry, m_Entities + m_Size, m_Entities + m_Size); }

	private:
		Registry* m_Registry;
		const Entity* m_Entities;
		size_t m_Size;
	};

	template<typename Owned, typename... Observed>
	class Group
	{
	public:
		Group(Registry* registry, ComponentArray<Owned>* ownedArray)
			: m_Registry(registry), m_OwnedArray(ownedArray) {}

		// Iterate owning component's entity list
		const Entity* begin() const { return m_OwnedArray->Entities(); }
		const Entity* end()   const { return m_OwnedArray->Entities() + m_OwnedArray->Size(); }

		std::tuple<Owned&, Observed&...> Get(Entity entity)
		{
			// Owning component is guaranteed to exist
			return {
				m_OwnedArray->GetData(entity),
				m_Registry->GetComponent<Observed>(entity)...
			};
		}

	private:
		Registry* m_Registry;
		ComponentArray<Owned>* m_OwnedArray;
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
		T& AddComponent(Entity entity, T component)
		{
			T& c = m_ComponentManager.AddComponent<T>(entity, component);

			auto signature = m_EntityManager.GetSignature(entity);
			signature.set(m_ComponentManager.GetComponentType<T>(), true);
			m_EntityManager.SetSignature(entity, signature);

			m_SystemManager.EntitySignatureChanged(entity, signature);

			return c;
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

		template<typename First, typename... Rest>
		View<First, Rest...> GetView()
		{
			auto storage = m_ComponentManager.GetComponentArray<First>();
			return View<First, Rest...>(
				this,
				storage->Entities(),
				storage->Size()
				);
		}

		template<typename Owned, typename... Observed>
		Group<Owned, Observed...> GetGroup()
		{
			auto ownedArray = m_ComponentManager.GetComponentArray<Owned>();

			for (size_t i = 0; i < ownedArray->Size(); )
			{
				Entity e = ownedArray->Entities()[i];

				if ((HasComponent<Observed>(e) && ...))
					++i;
				else
					ownedArray->RemoveData(e);
			}

			return Group<Owned, Observed...>(this, ownedArray.get());
		}

	private:
		ComponentManager m_ComponentManager;
		EntityManager	 m_EntityManager;
		SystemManager	 m_SystemManager;
	};
}