#pragma once
#include "Types.h"
#include <set>
#include <memory>
#include <unordered_map>

namespace Polarity::ECS
{

	class System
	{
	public:
		std::set<Entity> mEntities;
	};

	class SystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			if (m_Systems.find(typeName) != m_Systems.end())
			{
				POL_CORE_ERROR("ECS: RegisterSystem() failed, system is already registered!");
				return;
			}

			// Create a pointer to the system and return it so it can be used externally
			auto system = std::make_shared<T>();
			m_Systems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			if (m_Systems.find(typeName) != m_Systems.end())
			{
				POL_CORE_ERROR("ECS: SetSignature() failed, system is used before registered!");
				return;
			}

			// Set the signature for this system
			m_Signatures.insert({ typeName, signature });
		}

		void EntityDestroyed(Entity entity)
		{
			// Erase a destroyed entity from all system lists
			// mEntities is a set so no check needed
			for (auto const& pair : m_Systems)
			{
				auto const& system = pair.second;

				system->mEntities.erase(entity);
			}
		}

		void EntitySignatureChanged(Entity entity, Signature entitySignature)
		{
			// Notify each system that an entity's signature changed
			for (auto const& pair : m_Systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = m_Signatures[type];

				// Entity signature matches system signature - insert into set
				if ((entitySignature & systemSignature) == systemSignature)
				{
					system->mEntities.insert(entity);
				}
				// Entity signature does not match system signature - erase from set
				else
				{
					system->mEntities.erase(entity);
				}
			}
		}

	private:
		// Map from system type string pointer to a signature
		std::unordered_map<const char*, Signature> m_Signatures{};

		// Map from system type string pointer to a system pointer
		std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
	};
}