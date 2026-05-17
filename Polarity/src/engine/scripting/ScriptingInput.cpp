#include "polpch.h"
#include "ScriptingInput.h"

namespace Polarity
{
	void ScriptingInput::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e)
		{
			m_CurrentKeys[e.GetKeyCode()] = true;
			return false;
		});

		dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e)
		{
			m_CurrentKeys[e.GetKeyCode()] = false;
			return false;
		});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](auto& e)
		{
			m_CurrentMouse[e.GetMouseButton()] = true;
			return false;
		});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](auto& e)
		{
			m_CurrentMouse[e.GetMouseButton()] = false;
			return false;
		});

		dispatcher.Dispatch<MouseMovedEvent>([this](auto& e)
		{
			m_CurrentMousePos = { e.GetX(), e.GetY() };
			return false;
		});

		dispatcher.Dispatch<MouseScrolledEvent>([this](auto& e)
		{
			m_Scrolled.x += e.GetXOffset();
			m_Scrolled.y += e.GetYOffset();
			return false;
		});
	}

	void ScriptingInput::OnUpdate()
	{
		m_PreviousKeys = m_CurrentKeys;
		m_PreviousMouse = m_CurrentMouse;

		m_PreviousMousePos = m_CurrentMousePos;

		m_Scrolled = { 0.0f, 0.0f };
	}

	bool ScriptingInput::IsKeyDown(KeyCode key) const
	{
		return m_CurrentKeys[key];
	}

	bool ScriptingInput::IsKeyPressed(KeyCode key) const
	{
		return m_CurrentKeys[key] && !m_PreviousKeys[key];
	}

	bool ScriptingInput::IsKeyReleased(KeyCode key) const
	{
		return !m_CurrentKeys[key] && m_PreviousKeys[key];
	}

	bool ScriptingInput::IsMouseButtonDown(MouseCode button) const
	{
		return m_CurrentMouse[button];
	}

	bool ScriptingInput::IsMouseButtonPressed(MouseCode button) const
	{
		return m_CurrentMouse[button] && !m_PreviousMouse[button];
	}

	bool ScriptingInput::IsMouseButtonReleased(MouseCode button) const
	{
		return !m_CurrentMouse[button] && m_PreviousMouse[button];
	}

	glm::vec2 ScriptingInput::GetMouse() const
	{
		return m_CurrentMousePos;
	}

	glm::vec2 ScriptingInput::GetMouseDelta() const
	{
		return m_CurrentMousePos - m_PreviousMousePos;
	}

	float ScriptingInput::GetMouseX() const
	{
		return m_CurrentMousePos.x;
	}

	float ScriptingInput::GetMouseY() const
	{
		return m_CurrentMousePos.y;
	}

	float ScriptingInput::GetMouseScrolledX() const
	{
		return m_Scrolled.x;
	}

	float ScriptingInput::GetMouseScrolledY() const
	{
		return m_Scrolled.y;
	}
}