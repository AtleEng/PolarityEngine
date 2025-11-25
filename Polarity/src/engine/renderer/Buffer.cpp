#include "polpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLBuffer.h"

namespace Polarity {

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexBuffer>(size);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI!");
		return nullptr;
	}
}