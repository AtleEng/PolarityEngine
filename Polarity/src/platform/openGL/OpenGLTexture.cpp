#include "polpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Polarity
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_width(width), m_height(height)
	{
		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		LOG_ASSERT(data, "Failed to load image at: [%s]", path.c_str());

		//to make them unsigned
		m_width = width;
		m_height = height;

		if (channels == 4)
		{
			m_internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
		}

		LOG_ASSERT(m_internalFormat && m_dataFormat, "Format not supported !!!");


		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);


		stbi_image_free(data);
	}

	OpenGLTexture2D:: ~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_rendererID);
	}


	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		LOG_ASSERT(size == m_width * m_height * bpp, "Size of data is not matching texture !!!");
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_rendererID);
	}
}