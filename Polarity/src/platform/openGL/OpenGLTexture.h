#pragma once

#include <glad/glad.h>
#include "engine/renderer/Texture.h"

namespace Polarity
{

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_width;  };
		virtual uint32_t GetHeight() const override { return m_height; };

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot) const override;

	private:
		std::string m_path;
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};
}

