#pragma once
#include <Polarity.h>
#include <array>

#include "engine/renderer/EditorCamera.h"

#include "Panels/PanelManager.h"

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

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveAsScene();

	private:
		void ShowProfiler();

		void DrawMenubarPanel();
	private:

		EditorContext m_EditorContext;
		PanelManager m_PanelManager;

		std::string m_CurrentFilepath = "";
	};
}