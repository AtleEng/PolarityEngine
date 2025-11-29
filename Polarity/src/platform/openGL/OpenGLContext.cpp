#include "polpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Polarity
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		LOG_ASSERT(windowHandle, "Window handle is null!");
	}
	void OpenGLContext::Init()
	{
		POLARITY_PROFILE_FUNCTION();

		LOG_INFO("OpenGL initializing...");
		glfwMakeContextCurrent(m_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOG_ASSERT(status, "Failed to initialize glad!");
	}
	void OpenGLContext::SwapBuffers()
	{
		POLARITY_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}
}