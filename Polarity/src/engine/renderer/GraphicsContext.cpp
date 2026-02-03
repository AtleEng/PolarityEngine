#include "polpch.h"
#include "GraphicsContext.h"

#include "engine/renderer/Renderer.h"
#include "platform/openGL/OpenGLContext.h"
namespace Polarity
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}
}