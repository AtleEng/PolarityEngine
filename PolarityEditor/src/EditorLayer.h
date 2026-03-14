#pragma once
#include <Polarity.h>
#include <array>
#include <filesystem>

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


		// Menu

		// Project
		void NewProject();
		bool OpenProject();
		bool OpenProject(const std::filesystem::path& path);
		void SaveProject();

		// Scene

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveAsScene();

		// Edit

		void Undo();
		void Redon();
		void Cut();
		void Copy();
		void Paste();
		void Duplicate();
		void Delete();

		//void Options();

		void OnScenePlay();
		void OnSceneStop();
		//void Step();

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMousePressedEvent(MouseButtonPressedEvent& e);

		void ShowProfiler();

		void DrawMenubar();
		void DrawToolbar();
	private:

		EditorContext m_Context;
		PanelManager m_PanelManager;

		std::filesystem::path m_CurrentFilepath = "";

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;
	};
}