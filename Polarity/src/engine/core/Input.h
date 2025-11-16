#pragma once
#include "KeyCodes.h"

#include <glm/glm.hpp>

namespace Polarity {

	class Input
	{
	public:
		// return true if key is down
		static bool IsKeyPressed(KeyCode key);
		
		// return true if button is down
		static bool IsMouseButtonPressed(MouseCode button);

		// get mouse screen position
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}