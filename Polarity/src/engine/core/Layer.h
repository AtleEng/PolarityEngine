#pragma once

#include "Core.h"
#include "../events/Event.h"

namespace Polarity
{
	class POLARITY_API Layer
	{
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const char* GetName() const { return m_debugName; }

	protected:
		const char* m_debugName;
	};
}