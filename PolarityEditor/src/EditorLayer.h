#pragma once
#include <Polarity.h>
#include <array>

#include "CamController.h"

namespace Polarity 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep tS) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		bool OnKeyPressedEvent(KeyPressedEvent& event);

	private:
		CamController  m_cameraController;
		ShaderLibrary  m_shaderLibrary;

		std::array<Ref<SubTexture2D>, 5> m_spritemap;
		Ref<Texture2D> m_atlasTex;
		Ref<Texture2D> m_gridTex;
		Ref<Texture2D> m_logoTex;

		Ref<AudioSource> m_clickSound;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_viewportSize;
		Ref<Framebuffer> m_framebuffer;
	};


}