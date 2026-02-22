#include "polpch.h"
#include "UniformBuffer.h"

#include "engine/renderer/Renderer.h"
#include "platform/openGL/OpenGLUniformBuffer.h"

namespace Polarity
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}
}
