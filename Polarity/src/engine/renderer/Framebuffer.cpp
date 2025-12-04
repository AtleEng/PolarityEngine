#include "polpch.h"
#include "Framebuffer.h"

#include "Renderer.h"

#include "platform/openGL/OpenGLFramebuffer.h"

namespace Polarity
{

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
	}

	LOG_MAJOR_ERROR("Unknown RendererAPI!");
	return nullptr;
}
}