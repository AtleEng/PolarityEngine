#pragma once

#include "Entity.h"
#include "engine/scripting/ScriptingInput.h"

namespace Polarity {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		// Runs once at creation
		virtual void OnCreate() {}
		// Runs once at destruction
		virtual void OnDestroy() {}
		// Runs every frame
		virtual void OnUpdate(Timestep ts) {}

		ScriptingInput* m_Input = nullptr;
	private:
		Entity m_Entity;

		friend class Scene;
	};

}
