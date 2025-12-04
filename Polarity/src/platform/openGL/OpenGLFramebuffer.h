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

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_ColorAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
	};
}