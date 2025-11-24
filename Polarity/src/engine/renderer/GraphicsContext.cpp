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
		case RendererAPI::API::None:    LOG_ASSERT(false, "No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}