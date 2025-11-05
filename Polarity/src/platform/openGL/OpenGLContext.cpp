#include "polpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Polarity
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		LOG_ASSERT(windowHandle, "Window handle is null !!!");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOG_ASSERT(status, "Failed to initialize Glad !!!");

		LOG_INFO("OpenGL initializing...");
		LOG_INFO("  Vendor: %s", glGetString(GL_VENDOR));
		LOG_INFO("  Renderer: %s", glGetString(GL_RENDERER));
		LOG_INFO("  Version: %s", glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}