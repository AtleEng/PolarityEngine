#pragma once
#include "Types.h"
#include <typeindex>

namespace Polarity::ECS
{

	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};


	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		ComponentArray()
		{
			m_EntityToIndexArray.fill(INVALID_INDEX);
		}
		void InsertData(Entity entity, T component)
		{
			size_t index = m_EntityToIndexArray[entity];
			bool found = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (found)
			{
				POL_CORE_ERROR("ECS: InsertData() failed, component added to same entity more than once id: %d!", entity);
				return;
			}
			else
			{
				// Put new entry at end and update the array
				size_t newIndex = m_Size;
				m_EntityToIndexArray[entity] = newIndex;
				m_IndexToEntityArray[newIndex] = entity;
				m_ComponentArray[newIndex] = component;

				++m_Size;
			}
		}

		void RemoveData(Entity entity)
		{
			if (entity >= MAX_ENTITIES)
			{
				POL_CORE_ERROR("ECS: RemoveData() failed, invalid entity id: %d!", entity);
				return;
			}
			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index == INVALID_INDEX || index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				POL_CORE_ERROR("ECS: RemoveData() failed, tried removing non-existent component id: %d!", entity);
				return;
			}
			size_t indexOfLastElement = m_Size - 1;

			if (index != indexOfLastElement)
			{
				m_ComponentArray[index] = m_ComponentArray[indexOfLastElement];

				Entity entityOfLastElement = m_IndexToEntityArray[indexOfLastElement];
				m_EntityToIndexArray[entityOfLastElement] = index;
				m_IndexToEntityArray[index] = entityOfLastElement;
			}

			m_EntityToIndexArray[entity] = INVALID_INDEX;
			m_IndexToEntityArray[indexOfLastElement] = INVALID_ENTITY;

			--m_Size;
		}

		T& GetData(Entity entity)
		{

			if (entity >= MAX_ENTITIES || entity < 0)
			{
				POL_CORE_ERROR("ECS: GetData() failed, invalid entity id: %d!", entity);
				return m_ComponentArray[0]; // Safe fallback
			}
			if (m_EntityToIndexArray[entity] == INVALID_INDEX)
			{
				POL_CORE_ERROR("ECS: GetData() failed, entity has no component id:%d!", entity);
				return m_ComponentArray[0];
			}

			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				POL_CORE_ERROR("GetData() failed, tried retrieving non-existent component (entity:%d, ).", entity);
				return m_ComponentArray[0]; // Safe fallback
			}

			return m_ComponentArray[index];
		}

		bool HasComponent(Entity entity)
		{
			if (m_EntityToIndexArray[entity] == INVALID_INDEX || entity >= MAX_ENTITIES || entity < 0)
			{
				return false;
			}
			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				return false;
			}
			return true;
		}

		void EntityDestroyed(Entity entity) override
		{
			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				return;
			}

			RemoveData(entity);
		}

		size_t Size() const { return m_Size; }

		const Entity* Entities() const
		{
			return m_IndexToEntityArray.data();
		}

	private:
		

		std::array<T, MAX_ENTITIES> m_ComponentArray{};
		std::array<size_t, MAX_ENTITIES> m_EntityToIndexArray{};
		std::array<Entity, MAX_ENTITIES> m_IndexToEntityArray{};

		size_t m_Size = 0;
	};

	class ComponentManager
	{
	public:
		template<typename T>
		void RegisterComponent()
		{
			std::type_index type = typeid(T);

			if (m_ComponentTypes.find(type) != m_ComponentTypes.end())
			{
				POL_CORE_ERROR("ECS: RegisterComponent() failed, registering type more than once typeName: %i!", type);
				return;
			}

			m_ComponentTypes[type] = m_NextComponentType;
			m_ComponentArrays[type] = std::make_shared<ComponentArray<T>>();
			++m_NextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			std::type_index type = typeid(T);
			return m_ComponentTypes[type];
		}

		template<typename T>
		T& AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
			return GetComponentArray<T>()->GetData(entity);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return GetComponentArray<T>()->GetData(entity);
		}

		template<typename T>
		bool HasComponent(Entity entity)
		{
			return GetComponentArray<T>()->HasComponent(entity);
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : m_ComponentArrays)
			{
				pair.second->EntityDestroyed(entity);
			}
		}

		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			std::type_index type = typeid(T);

			if (m_ComponentTypes.find(type) == m_ComponentTypes.end()) //TODO maybe use typeIndex for preformance
			{
				POL_CORE_ERROR("ECS: GetComponentArray() failed, component not registered before use type: %i!", type);
				return nullptr;
			}

			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[type]);
		}

	private:
		std::unordered_map<std::type_index, ComponentType> m_ComponentTypes{};
		std::unordered_map<std::type_index, Ref<IComponentArray>> m_ComponentArrays{};
		ComponentType m_NextComponentType{};	
	};
}
