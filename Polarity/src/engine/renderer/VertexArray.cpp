#include "polpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLVertexArray.h"

namespace Polarity {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
		}

		POL_CORE_FATAL("Unknown RendererAPI !!!");
		return nullptr;
	}
}