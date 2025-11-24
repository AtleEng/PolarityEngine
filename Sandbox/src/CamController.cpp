#include "CamController.h"

Polarity::CamController::CamController(float aspectRatio)
	: m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel)
{
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
}

void Polarity::CamController::OnUpdate(Timestep ts)
{
	POLARITY_PROFILE_FUNCTION();

	if (Input::IsKeyPressed(Key::R))
	{
		m_camPos = glm::vec3(0.0f);
		m_zoomLevel = 1;
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	if (Input::IsMouseButtonPressed(Mouse::Button2))
	{
		glm::vec2 mP = m_camera.ScreenToWorld(Input::GetMousePosition());

		difference = (glm::vec3(mP.x, mP.y, 0.0f) - m_camPos);
		if (isPaning == false)
		{
			isPaning = true;
			origin = glm::vec3(mP.x, mP.y, 0.0f);
		}
	}
	else
	{
		isPaning = false;
	}
	if (isPaning)
	{
		m_camPos = origin - difference;
	}
	m_camera.SetPosition({ m_camPos });
}

void Polarity::CamController::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(POLARITY_BIND_EVENT_FN(OnMouseScrolled));
	dispatcher.Dispatch<WindowResizeEvent>(POLARITY_BIND_EVENT_FN(OnWindowResized));
}

void Polarity::CamController::OnResize(float width, float height)
{
	m_aspectRatio = width / height;
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
}

bool Polarity::CamController::OnMouseScrolled(MouseScrolledEvent& e)
{
	m_zoomLevel -= e.GetYOffset() * 0.25f;
	m_zoomLevel = std::max(m_zoomLevel, 0.25f);
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	return false;
}

bool Polarity::CamController::OnWindowResized(WindowResizeEvent& e)
{
	OnResize((float)e.GetWidth(), (float)e.GetHeight());
	return false;
}
