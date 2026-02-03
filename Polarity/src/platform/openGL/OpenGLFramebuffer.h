#pragma once

#include "engine/renderer/Framebuffer.h"

namespace Polarity
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int  ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		int  ReadPixels(uint32_t attachmentIndex, int x, int y, int sizeX, int sizeY) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override 
		{
			POL_CORE_ASSERT(index < m_ColorAttachments.size(), "OpenGL: Tried get m_ColorAttachment with invalid index %d!", index); 
			return m_ColorAttachments[index]; 
		}

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}