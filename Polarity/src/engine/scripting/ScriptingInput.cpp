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
	}

	void ScriptingInput::OnUpdate()
	{
		m_PreviousKeys = m_CurrentKeys;
		m_PreviousMouse = m_CurrentMouse;
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
}