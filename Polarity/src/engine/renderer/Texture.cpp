#include "polpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "platform/openGL/OpenGLTexture.h"

namespace Polarity
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		POL_CORE_INFO("Creating new texture (%i, %i)", width, height);
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("RendererAPI: None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(width, height);
		}

		POL_CORE_FATAL("Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		POL_CORE_INFO("Creating texture: %s", path.c_str());
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		POL_CORE_FATAL("RendererAPI: None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path);
		}

		POL_CORE_FATAL("Unknown RendererAPI!");
		return nullptr;
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2D> texture, const glm::vec2 coords, const glm::vec2 size)
		: m_texture(texture)
	{
		m_textureBounds[0] = { coords.x,			coords.y };
		m_textureBounds[1] = { coords.x + size.x,	coords.y };
		m_textureBounds[2] = { coords.x + size.x,	coords.y + size.y };
		m_textureBounds[3] = { coords.x,			coords.y + size.y };
	}
	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D> texture, const glm::vec2 coords, const glm::vec2 size)
	{
		glm::vec2 _coords = {
		coords.x / texture->GetWidth(),
		coords.y / texture->GetHeight()
		};

		glm::vec2 _size = {
			size.x / texture->GetWidth(),
			size.y / texture->GetHeight()
		};
		return CreateRef<SubTexture2D>(texture, _coords, _size);
	}
	//Calculate subTexture from a square grid 
	Ref<SubTexture2D> SubTexture2D::CreateFromUniformGrid(const Ref<Texture2D> texture, const glm::vec2 indexCoords, float gridSize)
	{
		float x = indexCoords.x * gridSize / texture->GetWidth();
		float y = indexCoords.y * gridSize / texture->GetHeight();
		float w = gridSize / texture->GetWidth();
		float h = gridSize / texture->GetHeight();

		glm::vec2 coords = { x, y };
		glm::vec2 size = { w, h };

		return CreateRef<SubTexture2D>(texture, coords, size);
	}
}