#pragma once

#include "engine/renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Polarity {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};

}