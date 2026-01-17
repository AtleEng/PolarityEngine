#include "EditorLayer.h"


#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Panels/AssetsPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/HierarcyPanel.h"
#include "Panels/ViewportPanel.h"

#include "engine/scene/SceneSerializer.h"
#include "engine/utils/PlatformUtils.h"

#include "UIIcons.h"

namespace Polarity
{
	EditorLayer::EditorLayer()
		: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f), m_PanelManager()
	{
	}

	void EditorLayer::OnAttach()
	{
		POLARITY_PROFILE_FUNCTION();

		UIIcons::Init();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/RobotoMono-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18.0f);

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_EditorContext.ViewportFramebuffer = Framebuffer::Create(fbSpec);
		m_EditorContext.ActiveScene = CreateRef<Scene>();

		// --------------------------------------------------- Panels

		m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<InspectorPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);

		auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_EditorContext);
		auto panelId = consolePanel.GetInstanceID();

		AddLogListener([this, panelId](const LogEvent& e)
		{
			if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
			{
				panel->BindLog(e);
			}
		});

		//-------------------------------------------------------- Entites
		m_CamEntity = m_EditorContext.ActiveScene->CreateEntity("Main Camera");
		m_CamEntity.AddComponent<CameraComponent>();

		m_SCamEntity = m_EditorContext.ActiveScene->CreateEntity("PreRender Camera");
		auto& cam = m_SCamEntity.AddComponent<CameraComponent>();
		cam.FixedAspectRatio = true;

		auto square = m_EditorContext.ActiveScene->CreateEntity();
		auto& sprite = square.AddComponent<SpriteComponent>();
		sprite.Color = { 1, 0, 1, 1 };

		class CamControll : public ScriptableEntity
		{
		public:
			float speed = 5.0f;

			void OnCreate()
			{

			}

			void OnDestroy()
			{

			}

			void OnUpdate(Timestep ts)
			{
				auto& position = GetComponent<TransformComponent>().Position;

				if (Input::IsKeyPressed(Key::A))
					position.x -= speed * ts;
				if (Input::IsKeyPressed(Key::D))
					position.x += speed * ts;
				if (Input::IsKeyPressed(Key::W))
					position.y += speed * ts;
				if (Input::IsKeyPressed(Key::S))
					position.y -= speed * ts;
			}
		private:
			float test = 1.0f;
		};

		m_CamEntity.AddComponent<ScriptComponent>().Bind<CamControll>();
		m_SCamEntity.AddComponent<ScriptComponent>().Bind<CamControll>();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep tS)
	{
		POLARITY_PROFILE_FUNCTION();

		auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
		if (viewport)
			viewport->UpdateViewport();

		//------------ Render ---------------------------------------
		{
			POLARITY_PROFILE_SCOPE("Render Draw");

			Renderer2D::ResetStats();

			m_EditorContext.ViewportFramebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			//------------ Scene -------------------------------------
			m_EditorContext.ActiveScene->OnUpdate(tS);

			m_EditorContext.ViewportFramebuffer->Unbind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		POLARITY_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;

		style.WindowMenuButtonPosition = -1; // cant find ImGui enum (none)
		style.TabRounding = 0;
		style.ScrollbarRounding = 0;
		style.ScrollbarSize = 6;

		ImVec4* colors = ImGui::GetStyle().Colors;
		{
			ImVec4 backgroundColor =					ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
			colors[ImGuiCol_WindowBg] =					backgroundColor;
			colors[ImGuiCol_ChildBg] =					backgroundColor;
			colors[ImGuiCol_PopupBg] =					backgroundColor;
			colors[ImGuiCol_TitleBg] =					backgroundColor;
			colors[ImGuiCol_TitleBgActive] =			backgroundColor;
			colors[ImGuiCol_TitleBgCollapsed] =			backgroundColor;
			colors[ImGuiCol_MenuBarBg] =				backgroundColor;
			colors[ImGuiCol_Tab] =						backgroundColor;
			colors[ImGuiCol_TabActive] =				backgroundColor;
			colors[ImGuiCol_TabUnfocused] =				backgroundColor;
			colors[ImGuiCol_TabUnfocusedActive] =		backgroundColor;
			colors[ImGuiCol_ScrollbarBg] =				backgroundColor;

			ImVec4 iteamBaseColor =						ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
			colors[ImGuiCol_FrameBg] =					iteamBaseColor;
			colors[ImGuiCol_Button] =					iteamBaseColor;
			colors[ImGuiCol_Header] =					iteamBaseColor;
			colors[ImGuiCol_TabHovered] =				iteamBaseColor;
			colors[ImGuiCol_ScrollbarGrab] =			iteamBaseColor;
			colors[ImGuiCol_SliderGrab] =				iteamBaseColor;
			colors[ImGuiCol_ResizeGrip] =				iteamBaseColor;

			ImVec4 iteamHoveredColor =					ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] =			iteamHoveredColor;
			colors[ImGuiCol_FrameBgActive] =			iteamHoveredColor;
			colors[ImGuiCol_ScrollbarGrabHovered] =		iteamHoveredColor;
			colors[ImGuiCol_ScrollbarGrabActive] =		iteamHoveredColor;
			colors[ImGuiCol_SliderGrabActive] =			iteamHoveredColor;
			colors[ImGuiCol_ButtonHovered] =			iteamHoveredColor;
			colors[ImGuiCol_ButtonActive] =				iteamHoveredColor;
			colors[ImGuiCol_HeaderHovered] =			iteamHoveredColor;
			colors[ImGuiCol_HeaderActive] =				iteamHoveredColor;
			colors[ImGuiCol_Separator] =				iteamHoveredColor;
			colors[ImGuiCol_SeparatorHovered] =			iteamHoveredColor;
			colors[ImGuiCol_SeparatorActive] =			iteamHoveredColor;
			colors[ImGuiCol_ResizeGripHovered] =		iteamHoveredColor;
			colors[ImGuiCol_ResizeGripActive] =			iteamHoveredColor;


			/*
			colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.13f, 0.94f);
			colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.03f, 0.03f, 0.03f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.14f, 0.15f, 0.40f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.67f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.06f, 0.06f, 0.51f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.16f, 0.17f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.24f, 0.61f, 0.55f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.29f, 0.32f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.42f, 0.44f, 1.00f);
			colors[ImGuiCol_Button] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.15f, 0.16f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.19f, 0.24f, 0.29f, 0.31f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.32f, 0.37f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.36f, 0.39f, 1.00f);
			colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.69f, 0.17f, 0.17f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.68f, 0.19f, 0.19f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.24f, 0.27f, 0.20f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.33f, 0.35f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.27f, 0.29f, 0.32f, 0.95f);
			colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.20f, 0.24f, 0.86f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.34f, 0.39f, 0.80f);
			colors[ImGuiCol_TabActive] = ImVec4(0.21f, 0.23f, 0.26f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.16f, 0.19f, 0.97f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.21f, 0.26f, 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.26f, 0.26f, 0.70f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.17f, 0.23f, 0.29f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.10f, 0.12f, 0.15f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			*/

		}


		ImGui::SetNextWindowPos({ viewport->WorkPos.x,  viewport->WorkPos.y });
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (ImGui::Begin("DockSpace", nullptr, window_flags))
		{
			DrawMenubarPanel();

			// Submit the DockSpace
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			ImGui::ShowDemoWindow();

			m_PanelManager.OnDraw();

			ImGui::End();
		}

	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));
		//m_cameraController.OnEvent(event);
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;
		bool ctrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (event.GetKeyCode())
		{
		case Key::N:
		{
			if (ctrl)
			{
				NewScene();
			}
			break;
		}
		case Key::O:
		{
			if (ctrl)
			{
				OpenScene();
			}
			break;
		}
		case Key::S:
		{
			if (ctrl)
			{
				if (shift)
				{
					SaveAsScene();
				}
				else
				{
					SaveScene();
				}
			}
			break;
		}
		case Key::Delete:
		{
			if (m_EditorContext.SelectedEntity.IsAlive())
			{
				m_EditorContext.ActiveScene->DestroyEntity(m_EditorContext.SelectedEntity.GetID());
			}
		}
		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		m_EditorContext.SelectedEntity = {};
		m_EditorContext.ActiveScene = CreateRef<Scene>();
		m_EditorContext.ViewportFramebuffer->Resize((uint32_t)m_EditorContext.ViewportSize.x, (uint32_t)m_EditorContext.ViewportSize.y);
		m_EditorContext.ActiveScene->OnViewportResize((uint32_t)m_EditorContext.ViewportSize.x, (uint32_t)m_EditorContext.ViewportSize.y);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Polarity Scene (*.pol)\0*.pol\0");
		if (!filepath.empty())
		{
			NewScene();

			SceneSerializer serializer(m_EditorContext.ActiveScene);
			serializer.DeSerialize(filepath);

			m_CurrentFilepath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_CurrentFilepath.empty())
		{
			SceneSerializer serializer(m_EditorContext.ActiveScene);
			serializer.Serialize(m_CurrentFilepath);

			LOG_INFO("Scene saved!");
		}
		else
		{
			SaveAsScene();
		}
	}

	void EditorLayer::SaveAsScene()
	{
		std::string filepath = FileDialogs::SaveFile("Polarity Scene (*.pol)\0*.pol\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_EditorContext.ActiveScene);
			serializer.Serialize(filepath);

			m_CurrentFilepath = filepath;
		}
	}


	void EditorLayer::ShowProfiler()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Profiler");

		// Title
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.85f, 0.4f, 1));
		ImGui::Text("PERFORMANCE");
		ImGui::PopStyleColor();
		ImGui::Separator();
		ImGui::Spacing();

		// CPU
		ImGui::Text("CPU");
		ImGui::Spacing();

		ImGui::Text("FPS        %.1f", io.Framerate);
		ImGui::Text("Delta Time %.4f", io.DeltaTime);

		// Renderer
		Renderer2D::Statistics stats = Renderer2D::GetStats();

		ImGui::Separator();
		ImGui::Text("Renderer");
		ImGui::Spacing();

		ImGui::Text("Draw Calls %d", stats.DrawCalls);
		ImGui::Text("Quads %d", stats.QuadCount);
		ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices %d", stats.GetTotalIndexCount());

		ImGui::End();
	}

	void EditorLayer::DrawMenubarPanel()
	{
		if (ImGui::BeginMainMenuBar())
		{
			float imageSize = 18.0f;
			float menuBarHeight = ImGui::GetFrameHeight();

			float cursorY = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(cursorY + (menuBarHeight - imageSize) * 0.5f);

			auto& plusIcon = UIIcons::Get(UIIcon::Logo);
			uint32_t texID = plusIcon->GetTexture()->GetRendererID();
			const glm::vec2* uvs = plusIcon->GetTexCoords();
			ImGui::Image(
				(void*)texID,
				ImVec2(imageSize, imageSize),
				ImVec2(uvs[0].x, uvs[0].y),
				ImVec2(uvs[2].x, uvs[2].y));
			

			// Restore Y pos
			ImGui::SetCursorPosY(cursorY);

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("New", "Ctrl + N"))
					NewScene();
				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S"))
					SaveAsScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) Application::Get().Shutdown();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Editor")) // all commands to edit project
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"));
				if (ImGui::MenuItem("Redo", "Ctrl+Y"));
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "Ctrl+X"));
				if (ImGui::MenuItem("Copy", "Ctrl+C"));
				if (ImGui::MenuItem("Paste", "Ctrl+V"));
				if (ImGui::MenuItem("Duplicate", "Ctrl+D"));
				ImGui::Separator();
				if (ImGui::MenuItem("Options TODO"));

				ImGui::Separator();
				ImGui::MenuItem("Play", "F5");
				ImGui::MenuItem("Simulate", "Shift+F5");

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Show"))
				{
					if (ImGui::MenuItem("Grid"));

					ImGui::Separator();
					if (ImGui::MenuItem("Gizmos"));

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Open"))
				{

					if (ImGui::MenuItem("Hierarcy"))
						m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);

					if (ImGui::MenuItem("Viewport"))
						m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);

					if (ImGui::MenuItem("Inspector"))
						m_PanelManager.OpenPanel<InspectorPanel>(m_EditorContext);

					if (ImGui::MenuItem("Assets"))
						m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);

					if (ImGui::MenuItem("Console"))
					{

						auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_EditorContext);
						auto panelId = consolePanel.GetInstanceID();

						AddLogListener([this, panelId](const LogEvent& e)
						{
							if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
							{
								panel->BindLog(e);
							}
						});
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Layout"))
				{
					m_PanelManager.Clear();
					m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<InspectorPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("General");
				ImGui::EndMenu();
			}
			ImGui::TextDisabled(m_CurrentFilepath.c_str());

			ImGui::EndMainMenuBar();
		}
	}
}
