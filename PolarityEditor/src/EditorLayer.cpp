#include "EditorLayer.h"


#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

#include "Panels/AssetsPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/PropertitiesPanel.h"
#include "Panels/HierarcyPanel.h"
#include "Panels/ViewportPanel.h"

#include "EditorEvents.h"

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
		POL_PROFILE_FUNCTION();

		UIIcons::Init();

		ImGuiIO& io = ImGui::GetIO();
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18.0f);

		FramebufferSpecification viewportFbSpec;
		viewportFbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		viewportFbSpec.Width = 1;
		viewportFbSpec.Height = 1;

		m_Context.ViewportFramebuffer = Framebuffer::Create(viewportFbSpec);

		m_Context.ActiveScene = CreateRef<Scene>();
		m_Context.EditorScene = CreateRef<Scene>();
		m_Context.RuntimeScene = CreateRef<Scene>();

		m_Context.EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_Context.EventCallback = POLARITY_BIND_EVENT_FN(OnEvent);

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			if (!OpenProject())
			{
				Application::Get().Shutdown();
			}
		}

		// --------------------------------------------------- Panels

		m_PanelManager.OpenPanel<HierarcyPanel>(m_Context);
		m_PanelManager.OpenPanel<ViewportPanel>(m_Context);
		m_PanelManager.OpenPanel<PropertitiesPanel>(m_Context);
		m_PanelManager.OpenPanel<AssetsPanel>(m_Context);

		auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_Context);
		auto panelId = consolePanel.GetInstanceID();

		Logger::AddLogListener([this, panelId](const LogEvent& e)
		{
			if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
			{
				panel->BindLog(e);
			}
		});

		//-------------------------------------------------------- Entites
		
		/*
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

		*/
		//camEntity.AddComponent<ScriptComponent>().Bind<CamControll>();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		POL_PROFILE_FUNCTION();


		
		//------------ Render ---------------------------------------
		{
			POLARITY_PROFILE_SCOPE("Render Draw");

			Renderer2D::ResetStats();

			m_Context.ViewportFramebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			m_Context.ViewportFramebuffer->ClearAttachment(1, -1);

			//------------ Scene -------------------------------------
			switch (m_SceneState)
			{
			case Polarity::EditorLayer::SceneState::Edit:
			{
				auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
				if (viewport)
				{
					viewport->UpdateViewport(m_Context.EditorCamera);
					if (viewport->IsFocused() || viewport->IsHovered())
					{
						m_Context.EditorCamera.OnUpdate(ts);
					}
				}

				m_Context.ActiveScene->OnUpdateEditor(ts, m_Context.EditorCamera);
				break;
			}
			case Polarity::EditorLayer::SceneState::Play:
			{
				m_Context.ActiveScene->OnUpdateRuntime(ts);
				break;
			}
			}

			OnOverlayRender();

			m_Context.ViewportFramebuffer->Unbind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		POL_PROFILE_FUNCTION();


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
			DrawMenubar();

			// Submit the DockSpace
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 260.0f;

			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			//ImGui::ShowDemoWindow();

			m_PanelManager.OnDraw();

			DrawToolbar();

			ImGui::End();
		}

	}

	void EditorLayer::OnEvent(Event& event)
	{
		auto viewport = m_PanelManager.GetPanel<ViewportPanel>();
		if (viewport && (viewport->IsHovered() || viewport->IsFocused()))
		{
			m_Context.EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));
		dispatcher.Dispatch<MouseButtonPressedEvent>(POLARITY_BIND_EVENT_FN(OnMousePressedEvent));
		dispatcher.Dispatch<OpenSceneEditorEvent>([this](OpenSceneEditorEvent& e)
		{
			OpenScene(e.GetPath());
			return true;
		});
	}

	//Handle all key commands
	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;
		bool ctrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
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
				if (shift)
				{
					OpenProject();
				}
				else
				{
					OpenScene();
				}
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
			Delete();
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
		case Key::F2:
		{
			POL_CORE_WARN("Settings not added");
			break;
		}
		case Key::F5:
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else
			{
				OnSceneStop();
			}
			break;
		}
		}

		return false;
	}

	bool EditorLayer::OnMousePressedEvent(MouseButtonPressedEvent& e)
	{
		m_PanelManager.OnMousePressedEvent(e);
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_Context.ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_Context.EditorCamera);
		}

		// Draw selected entity outline TODO Change to line renderer
		
		if (Entity selectedEntity = m_Context.GetSelected())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawQuad(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}
		
		Renderer2D::EndScene();
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Polarity Project (*.proj)\0*.proj\0");
		if (!filepath.empty())
		{
			return OpenProject(filepath);
		}
		return false;
	}

	bool EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path))
		{
			auto startScenePath = Project::GetAssetDirectory() / Project::GetActive()->GetConfig().StartScene;
			OpenScene(startScenePath);
			return true;
		}
		return false;
	}

	void EditorLayer::SaveProject()
	{
		// Project::SaveActive();
	}


	void EditorLayer::NewScene()
	{
		m_Context.SetSelected({});
		m_Context.ActiveScene = CreateRef<Scene>();
		m_Context.ViewportFramebuffer->Resize((uint32_t)m_Context.ViewportSize.x, (uint32_t)m_Context.ViewportSize.y);
		m_Context.ActiveScene->OnViewportResize((uint32_t)m_Context.ViewportSize.x, (uint32_t)m_Context.ViewportSize.y);
		m_Context.EditorScene = m_Context.ActiveScene;


		m_EditorSceneFilepath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Polarity Scene (*.pol)\0*.pol\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
		POL_CORE_ERROR("Scene: %s failed to deserialize!", filepath.c_str());
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		std::string filename = path.filename().string();
		if (path.extension().string() != ".pol")
		{
			POL_CORE_WARN("Could not load %s - not a scene file '.pol'", filename.c_str());
			return;
		}

		Ref<Scene> scene = CreateRef<Scene>();
		scene->OnViewportResize((uint32_t)m_Context.ViewportSize.x, (uint32_t)m_Context.ViewportSize.y);

		SceneSerializer serializer(scene);
		if (serializer.DeSerialize(path.string()))
		{
			m_Context.EditorScene = scene;
			m_Context.ActiveScene = m_Context.EditorScene;
		
			m_EditorSceneFilepath = path;

			POL_CORE_INFO("Scene: %s opened!", filename.c_str());
		}
		else
		{
			POL_CORE_ERROR("Scene: %s failed to deserialize!", filename.c_str());
		}

	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorSceneFilepath.empty())
		{
			SceneSerializer serializer(m_Context.EditorScene);
			serializer.Serialize(m_EditorSceneFilepath.string());

			POL_CORE_INFO("Scene: %s saved!", m_EditorSceneFilepath.filename().string().c_str());
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
			m_EditorSceneFilepath = filepath;
			SaveScene();
		}
		else
		{
			POL_CORE_WARN("Scene not saved, has no filepath!");
		}
	}

	void EditorLayer::Undo()
	{
	}

	void EditorLayer::Redon()
	{
	}

	void EditorLayer::Cut()
	{
	}

	void EditorLayer::Copy()
	{
	}

	void EditorLayer::Paste()
	{
	}

	void EditorLayer::Duplicate()
	{
	}

	void EditorLayer::Delete()
	{
		if (m_Context.GetSelected().IsAlive())
		{
			m_Context.ActiveScene->DestroyEntity(m_Context.GetSelected().GetHandle());
		}
	}

	void EditorLayer::OnScenePlay()
	{
		POL_CORE_TRACE("Scene Playing...");
		m_SceneState = SceneState::Play;

		m_Context.SetSelected({});
		m_Context.ActiveScene = Scene::Copy(m_Context.EditorScene);
		m_Context.ActiveScene->OnRuntimeStart();
	}

	void EditorLayer::OnSceneStop()
	{
		m_Context.ActiveScene->OnRuntimeStop();
		m_SceneState = SceneState::Edit;

		m_Context.SetSelected({});
		m_Context.ActiveScene = m_Context.EditorScene;

		POL_CORE_TRACE("Scene Stopped");
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

	void EditorLayer::DrawMenubar()
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

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save Scene as...", "Ctrl+Shift+S"))
					SaveAsScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Open Project...", "Ctrl+Shift+O"))
					OpenProject();
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4")) Application::Get().Shutdown();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo TODO", "Ctrl+Z"));
				if (ImGui::MenuItem("Redo TODO", "Ctrl+Y"));
				ImGui::Separator();
				if (ImGui::MenuItem("Cut TODO", "Ctrl+X"));
				if (ImGui::MenuItem("Copy TODO", "Ctrl+C"));
				if (ImGui::MenuItem("Paste TODO", "Ctrl+V"));
				if (ImGui::MenuItem("Duplicate TODO", "Ctrl+D"));
				if (ImGui::MenuItem("Delete TODO", "Del"));
				ImGui::Separator();
				if (ImGui::MenuItem("Options TODO", "F2"));

				ImGui::Separator();
				
				if (ImGui::MenuItem("Play/Stop", "F5"))
				{
					if (m_SceneState == SceneState::Edit)
					{
						OnScenePlay();
					}
					else
					{
						OnSceneStop();
					}
				}
				

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
					m_PanelManager.OpenPanel<HierarcyPanel>(m_Context);

				if (ImGui::MenuItem("Viewport"))
					m_PanelManager.OpenPanel<ViewportPanel>(m_Context);

				if (ImGui::MenuItem("Properties"))
					m_PanelManager.OpenPanel<PropertitiesPanel>(m_Context);

				if (ImGui::MenuItem("Assets"))
					m_PanelManager.OpenPanel<AssetsPanel>(m_Context);

				if (ImGui::MenuItem("Console"))
				{

					auto& consolePanel = m_PanelManager.OpenPanel<ConsolePanel>(m_Context);
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
					m_PanelManager.OpenPanel<HierarcyPanel>(m_Context);
					m_PanelManager.OpenPanel<ViewportPanel>(m_Context);
					m_PanelManager.OpenPanel<PropertitiesPanel>(m_Context);
					m_PanelManager.OpenPanel<AssetsPanel>(m_Context);
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

	void EditorLayer::DrawToolbar()
	{
		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		Ref<SubTexture2D> icon = UIIcons::Get(UIIcon::Play);

		if (m_SceneState == SceneState::Play)
		{
			icon = UIIcons::Get(UIIcon::Pause);

		}


		uint32_t texID = icon->GetTexture()->GetRendererID();
		const glm::vec2* uvs = icon->GetTexCoords();

		if (ImGui::ImageButton((void*)texID,
			{ 18,18 },
			ImVec2(uvs[0].x, uvs[2].y),
			ImVec2(uvs[2].x, uvs[0].y)))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}
		ImGui::End();
	}
}
