#pragma once
#include <Polarity.h>
#include <glm/glm.hpp>

namespace Polarity {

	class CamController
	{
	public:
		CamController(float aspectRatio = 1280.0f / 720.0f);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_camera; }
		const OrthographicCamera& GetCamera() const { return m_camera; }

		float GetZoomLevel() const { return m_zoomLevel; }
		void SetZoomLevel(float level) { m_zoomLevel = level; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		OrthographicCamera m_camera;
		glm::vec3 m_camPos = glm::vec3(0.0f);

		float m_aspectRatio;
		float m_zoomLevel = 1.0f;

		glm::vec3 origin = glm::vec3(0.0f);
		glm::vec3 difference = glm::vec3(0.0f);
		glm::vec3 resetCameraPos = glm::vec3(0.0f);

		bool isPaning = false;
	};

}
