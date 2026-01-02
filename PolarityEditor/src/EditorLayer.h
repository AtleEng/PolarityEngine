#pragma once
#include <Polarity.h>
#include <array>

#include "CamController.h"
#include "Panels/EditorPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/MenubarPanel.h"

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
		void ShowProfiler  ();
	private:
		CamController  m_cameraController;

		Ref<Texture2D> m_logoTex;

		Entity m_CamEntity;
		Entity m_SCamEntity;


		EditorContext m_EditorContext;
	};
}