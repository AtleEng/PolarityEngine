#pragma once
#include "Types.h"

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
				LOG_ERROR("InsertData() failed, component added to same entity more than once (%d).", entity);
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
			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				LOG_ERROR("RemoveData() failed, tried removing non-existent component (%d).", entity);
				return;
			}

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = index;
			size_t indexOfLastElement = m_Size - 1;
			m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

			// Update arrays
			Entity entityOfLastElement = m_IndexToEntityArray[indexOfLastElement];
			m_EntityToIndexArray[entityOfLastElement] = indexOfRemovedEntity;
			m_IndexToEntityArray[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndexArray[entity] = INVALID_INDEX;

			--m_Size;
		}

		T& GetData(Entity entity)
		{
			if (entity >= MAX_ENTITIES || entity < 0)
			{
				LOG_ERROR("GetData() failed, invalid entity ID (entity:%d).", entity);
				return m_ComponentArray[0]; // Safe fallback
			}
			if (m_EntityToIndexArray[entity] == INVALID_INDEX)
			{
				LOG_ERROR("GetData() failed, entity has no component (entity:%d).", entity);
				return m_ComponentArray[0];
			}

			size_t index = m_EntityToIndexArray[entity];
			bool exists = (index < m_Size&& m_IndexToEntityArray[index] == entity);

			if (!exists)
			{
				LOG_ERROR("GetData() failed, tried retrieving non-existent component (entity:%d, ).", entity);
				return m_ComponentArray[0]; // Safe fallback
			}

			return m_ComponentArray[index];
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

	private:
		static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

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
			const char* typeName = typeid(T).name();

			if (m_ComponentTypes.find(typeName) != m_ComponentTypes.end())
			{
				LOG_ERROR("RegisterComponent() failed, registering type more than once (%s).", typeName);
				return;
			}

			m_ComponentTypes[typeName] = m_NextComponentType;
			m_ComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
			++m_NextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();
			return m_ComponentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
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

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : m_ComponentArrays)
			{
				pair.second->EntityDestroyed(entity);
			}
		}

	private:
		std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{};
		ComponentType m_NextComponentType{};

		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();

			if (m_ComponentTypes.find(typeName) == m_ComponentTypes.end())
			{
				LOG_ERROR("GetComponentArray() failed, component not registered before use.");
				return nullptr;
			}

			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
		}
	};
}
