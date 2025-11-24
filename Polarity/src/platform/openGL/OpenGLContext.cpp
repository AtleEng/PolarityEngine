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

		
		LOG_INFO(" - Vendor:   %s", glGetString(GL_VENDOR));
		LOG_INFO(" - Renderer: %s", glGetString(GL_RENDERER));
		LOG_INFO(" - Version:  %s", glGetString(GL_VERSION));
		LOG_INFO("OpenGL done\n");
	}
	void OpenGLContext::SwapBuffers()
	{
		POLARITY_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}
}