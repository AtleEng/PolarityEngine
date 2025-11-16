#include "polpch.h"
#include "OrthographicCamera.h"

#include "../core/Application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Polarity {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_viewMatrix(1.0f)
	{
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}
	const glm::vec2& OrthographicCamera::ScreenToWorld(glm::vec2 position)
	{
		float screenHeight	 = Application::Get().GetWindow().GetHeight();
		float screenWidth	 = Application::Get().GetWindow().GetWidth();

		float x = (2.0f * position.x) / screenWidth - 1.0f;
		float y = 1.0f - (2.0f * position.y) / screenHeight; // Flip Y

		glm::vec4 ndc = glm::vec4(x, y, 0.0f, 1.0f);

		glm::mat4 invVP = glm::inverse(m_viewProjectionMatrix);
		glm::vec4 world = invVP * ndc;

		return glm::vec2(world.x, world.y);
	}
	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}
}