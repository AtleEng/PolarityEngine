#pragma once

#include "engine/core/Log.h"
#include "engine/core/Keycodes.h"
#include "engine/events/Event.h"
#include "engine/events/KeyEvent.h"
#include "engine/events/MouseEvent.h"

#include <array>
#include <glm/glm.hpp>

namespace Polarity {

	class ScriptingInput
	{
	public:

		void OnEvent(Event& e);
		void OnUpdate();

		bool IsKeyDown(KeyCode key) const;
		bool IsKeyPressed(KeyCode key) const;
		bool IsKeyReleased(KeyCode key) const;

		bool IsMouseButtonDown(MouseCode button) const;
		bool IsMouseButtonPressed(MouseCode button) const;     
		bool IsMouseButtonReleased(MouseCode button) const;

		// get mouse screen position
		glm::vec2 GetMousePosition();
		float GetMouseX();
		float GetMouseY();

	private:
		std::array<bool, 512> m_CurrentKeys{};
		std::array<bool, 512> m_PreviousKeys{};

		std::array<bool, 16> m_CurrentMouse{};
		std::array<bool, 16> m_PreviousMouse{};
	};
}