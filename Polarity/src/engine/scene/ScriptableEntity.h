#pragma once

#include "Entity.h"

namespace Polarity {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
			//return m_Scene->GetRegistry().GetComponent(m_Entity.GetHandle());
		}

	protected:
		// Runs once at creation
		virtual void OnCreate() {}
		// Runs once at destruction
		virtual void OnDestroy() {}
		// Runs every frame
		virtual void OnUpdate(Timestep ts) {}
	private:
		Entity m_Entity;
		Ref<Scene> m_Scene;

		friend class Scene;
	};

}
