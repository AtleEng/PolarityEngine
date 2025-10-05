#pragma once

#include "engine/Layer.h"

#include "engine/events/ApplicationEvent.h"
#include "engine/events/KeyEvent.h"
#include "engine/events/MouseEvent.h"

namespace Atlas
{
	class ATLAS_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		
		bool OnWindowResizeEvent(WindowResizeEvent& e);

		float m_time = 0;
	};
}
