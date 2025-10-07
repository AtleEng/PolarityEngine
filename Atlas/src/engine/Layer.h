#pragma once

#include "engine/Core.h"
#include "engine/events/Event.h"

namespace Atlas
{
	class ATLAS_API Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const char* GetName() const { return m_debugName; }

	protected:
		const char* m_debugName;
	};
}