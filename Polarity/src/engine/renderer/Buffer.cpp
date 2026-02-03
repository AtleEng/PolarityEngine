#include "polpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLBuffer.h"

namespace Polarity {

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(size);
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}
}