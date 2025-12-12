#pragma once
#include "Types.h"

namespace Polarity::ECS
{
	class EntityManager
	{
	public:
		EntityManager();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

		void SetSignature(Entity entity, Signature signature);
		Signature GetSignature(Entity entity);

		int GetEntityAmount();

	private:
		// Queue of unused entity IDs
		std::queue<Entity> m_AvailableEntities{};

		// Array of signatures where the index corresponds to the entity ID
		std::array<Signature, MAX_ENTITIES> m_Signatures{};

		// Total living entities - used to keep limits on how many exist
		uint32_t m_LivingEntityCount{};
	};
}