#pragma once

#include "engine/core/Application.h"
#include "engine/ECS/Registry.h"

namespace Polarity {
	class Scene;

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Scene->GetRegistry().GetComponent<T>(m_EntityHandle);
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
	private:
		Scene* m_Scene = nullptr;
		ECS::Entity m_EntityHandle{ ECS::INVALID_ENTITY };

		friend class Scene;
	};

}
