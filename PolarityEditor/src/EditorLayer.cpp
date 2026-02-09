#include "EditorLayer.h"


#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

#include "Panels/AssetsPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/PropertitiesPanel.h"
#include "Panels/HierarcyPanel.h"
#include "Panels/ViewportPanel.h"

#include "engine/scene/SceneSerializer.h"
#include "engine/utils/PlatformUtils.h"

#include "UIIcons.h"

namespace Polarity
{
	EditorLayer::EditorLayer()
		: Layer("DemoLayer"), m_PanelManager()
	{
	}

	void EditorLayer::OnAttach()
	{
		POLARITY_PROFILE_FUNCTION();

		UIIcons::Init();

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/RobotoMono-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18.0f);


		FramebufferSpecification viewportFbSpec;
		viewportFbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		viewportFbSpec.Width = 1280;
		viewportFbSpec.Height = 720;
		m_EditorContext.ViewportFramebuffer = Framebuffer::Create(viewportFbSpec);

		FramebufferSpecification previewFbSpec;
		previewFbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		previewFbSpec.Width = 320;
		previewFbSpec.Height = 180;
		m_EditorContext.PreviewFramebuffer = Framebuffer::Create(previewFbSpec);

		m_EditorContext.ActiveScene = CreateRef<Scene>();
		m_EditorContext.EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		// --------------------------------------------------- Panels

		m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<PropertitiesPanel>(m_EditorContext);
		m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);

		auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_EditorContext);
		auto panelId = consolePanel.GetInstanceID();

		Logger::AddLogListener([this, panelId](const LogEvent& e)
		{
			if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
			{
				panel->BindLog(e);
			}
		});

		//-------------------------------------------------------- Entites
		Entity camEntity = m_EditorContext.ActiveScene->CreateEntity("Main Camera");
		camEntity.AddComponent<CameraComponent>();

		Entity sCamEntity = m_EditorContext.ActiveScene->CreateEntity("PreRender Camera");
		auto& cam = sCamEntity.AddComponent<CameraComponent>();
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

		camEntity.AddComponent<ScriptComponent>().Bind<CamControll>();
		sCamEntity.AddComponent<ScriptComponent>().Bind<CamControll>();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		POLARITY_PROFILE_FUNCTION();

		auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
		if (viewport)
		{
			viewport->UpdateViewport(m_EditorContext.EditorCamera);
			if (viewport->IsFocused() || viewport->IsHovered())
			{
				m_EditorContext.EditorCamera.OnUpdate(ts);
			}
		}

		//------------ Render ---------------------------------------
		{
			POLARITY_PROFILE_SCOPE("Render Draw");

			Renderer2D::ResetStats();

			m_EditorContext.ViewportFramebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			m_EditorContext.ViewportFramebuffer->ClearAttachment(1, -1);
			//------------ Scene -------------------------------------
			m_EditorContext.ActiveScene->OnUpdateEditor(ts, m_EditorContext.EditorCamera);

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
			ImVec4 backgroundColor = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
			colors[ImGuiCol_WindowBg] = backgroundColor;
			colors[ImGuiCol_ChildBg] = backgroundColor;
			colors[ImGuiCol_PopupBg] = backgroundColor;
			colors[ImGuiCol_TitleBg] = backgroundColor;
			colors[ImGuiCol_TitleBgActive] = backgroundColor;
			colors[ImGuiCol_TitleBgCollapsed] = backgroundColor;
			colors[ImGuiCol_MenuBarBg] = backgroundColor;
			colors[ImGuiCol_Tab] = backgroundColor;
			colors[ImGuiCol_TabActive] = backgroundColor;
			colors[ImGuiCol_TabUnfocused] = backgroundColor;
			colors[ImGuiCol_TabUnfocusedActive] = backgroundColor;
			colors[ImGuiCol_ScrollbarBg] = backgroundColor;

			ImVec4 iteamBaseColor = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
			colors[ImGuiCol_FrameBg] = iteamBaseColor;
			colors[ImGuiCol_Button] = iteamBaseColor;
			colors[ImGuiCol_Header] = iteamBaseColor;
			colors[ImGuiCol_TabHovered] = iteamBaseColor;
			colors[ImGuiCol_ScrollbarGrab] = iteamBaseColor;
			colors[ImGuiCol_SliderGrab] = iteamBaseColor;
			colors[ImGuiCol_ResizeGrip] = iteamBaseColor;

			ImVec4 iteamHoveredColor = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] = iteamHoveredColor;
			colors[ImGuiCol_FrameBgActive] = iteamHoveredColor;
			colors[ImGuiCol_ScrollbarGrabHovered] = iteamHoveredColor;
			colors[ImGuiCol_ScrollbarGrabActive] = iteamHoveredColor;
			colors[ImGuiCol_SliderGrabActive] = iteamHoveredColor;
			colors[ImGuiCol_ButtonHovered] = iteamHoveredColor;
			colors[ImGuiCol_ButtonActive] = iteamHoveredColor;
			colors[ImGuiCol_HeaderHovered] = iteamHoveredColor;
			colors[ImGuiCol_HeaderActive] = iteamHoveredColor;
			colors[ImGuiCol_Separator] = iteamHoveredColor;
			colors[ImGuiCol_SeparatorHovered] = iteamHoveredColor;
			colors[ImGuiCol_SeparatorActive] = iteamHoveredColor;
			colors[ImGuiCol_ResizeGripHovered] = iteamHoveredColor;
			colors[ImGuiCol_ResizeGripActive] = iteamHoveredColor;
		}

		ImGui::SetNextWindowPos({ viewport->WorkPos.x,  viewport->WorkPos.y });
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (ImGui::Begin("DockSpace", nullptr, window_flags))
		{
			DrawMenubarPanel();

			// Submit the DockSpace
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 260.0f;

			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			//ImGui::ShowDemoWindow();

			m_PanelManager.OnDraw();

			ImGui::End();
		}

	}

	void EditorLayer::OnEvent(Event& event)
	{
		auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
		if (viewport && (viewport->IsHovered() || viewport->IsFocused()))
		{
			m_EditorContext.EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));
	}

	//Handle all key commands
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
			break;
		}
		case Key::Q:
		{
			auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
			if (viewport)
			{
				viewport->SetGizmoOperation(GizmoOperation::Bounds);
			}
			break;
		}
		case Key::W:
		{
			auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
			if (viewport)
			{
				viewport->SetGizmoOperation(GizmoOperation::Translate);
			}
			break;
		}
		case Key::E:
		{
			auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
			if (viewport)
			{
				viewport->SetGizmoOperation(GizmoOperation::Rotate);
			}
			break;
		}
		case Key::R:
		{
			auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
			if (viewport)
			{
				viewport->SetGizmoOperation(GizmoOperation::Scale);
			}
			break;
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

			POL_CORE_INFO("Scene saved!");
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

			auto& logoIcon = UIIcons::Get(UIIcon::Logo);
			uint32_t texID = logoIcon->GetTexture()->GetRendererID();
			const glm::vec2* uvs = logoIcon->GetTexCoords();
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
				if (ImGui::MenuItem("Delete", "Del"));
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
				ImGui::Separator();

				if (ImGui::MenuItem("Hierarcy"))
					m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);

				if (ImGui::MenuItem("Viewport"))
					m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);

				if (ImGui::MenuItem("Properties"))
					m_PanelManager.OpenPanel<PropertitiesPanel>(m_EditorContext);

				if (ImGui::MenuItem("Assets"))
					m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);

				if (ImGui::MenuItem("Console"))
				{

					auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_EditorContext);
					auto panelId = consolePanel.GetInstanceID();

					Logger::AddLogListener([this, panelId](const LogEvent& e)
					{
						if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
						{
							panel->BindLog(e);
						}
					});
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Layout"))
				{
					m_PanelManager.Clear();
					m_PanelManager.OpenPanel<HierarcyPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<ViewportPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<PropertitiesPanel>(m_EditorContext);
					m_PanelManager.OpenPanel<AssetsPanel>(m_EditorContext);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("General");
				ImGui::EndMenu();
			}

			//ImGui::TextDisabled(m_CurrentFilepath.c_str());

			ImGui::EndMainMenuBar();
		}
	}
}
