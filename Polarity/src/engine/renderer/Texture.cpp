#include "polpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLTexture.h"

namespace Polarity
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(width, height);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI !!!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		LOG_MAJOR_ERROR("RendererAPI: None is currently not supported !!!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path);
		}

		LOG_MAJOR_ERROR("Unknown RendererAPI !!!");
		return nullptr;
	}
}