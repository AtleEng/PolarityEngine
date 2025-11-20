#pragma once

#include "Core.h"
#include "Timestep.h"
#include "../events/Event.h"

namespace Polarity
{
	class Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep tS) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const char* GetName() const { return m_debugName; }

	protected:
		const char* m_debugName;
	};
}