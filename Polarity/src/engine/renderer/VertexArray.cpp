#include "polpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLVertexArray.h"

namespace Polarity {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("Renderer: No RendererAPI!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
		}

		POL_CORE_FATAL("Renderer: Unknown RendererAPI!");
		return nullptr;
	}
}