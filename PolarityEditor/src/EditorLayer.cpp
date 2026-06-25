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
#include "engine/scripting/ScriptingEngine.h"
#include "engine/utils/PlatformUtils.h"

#include "UIIcons.h"
#include "EditorTheme.h"

namespace Polarity
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_PanelManager()
	{
	}

	void EditorLayer::OnAttach()
	{
		POL_PROFILE_FUNCTION();

		UIIcons::Init();
		EditorTheme::Load("assets/themes/Default.txt");

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

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		POL_PROFILE_FUNCTION();

		std::filesystem::path dllPath = Project::GetProjectDirectory();
		dllPath /= "bin";
		dllPath /= "Game.dll";

		if (ScriptEngine::Update(dllPath))
		{
			OpenScene(m_EditorSceneFilepath.string());
		}
		//------------ Render ---------------------------------------
		{
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
		/*
		style.WindowMenuButtonPosition = 0; // cant find ImGui enum (none)
		style.TabRounding = 0;
		style.ScrollbarRounding = 0;
		style.ScrollbarSize = 6;
		*/
		style.FrameBorderSize = 1;

		EditorTheme::Apply();

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

			ImGui::ShowDemoWindow();

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
			POL_WARN("Settings not added");
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

		if (m_DrawGrid)
		{
			float axisLength = 10.0f;

			// X axis (red)
			Renderer2D::DrawLine(
				glm::vec3(-axisLength, 0.0f, 0.0f),
				glm::vec3(axisLength, 0.0f, 0.0f),
				glm::vec4(1, 0, 0, 0.1f)
			);

			// Y axis (Blue)
			Renderer2D::DrawLine(
				glm::vec3(0.0f, -axisLength, 0.0f),
				glm::vec3(0.0f, axisLength, 0.0f),
				glm::vec4(0, 0, 1, 0.1f)
			);

			// Z axis (green)
			Renderer2D::DrawLine(
				glm::vec3(0.0f, 0.0f, -axisLength),
				glm::vec3(0.0f, 0.0f, axisLength),
				glm::vec4(0, 1, 0, 0.1f)
			);
		}

		// Draw selected entity outline

		if (Entity selectedEntity = m_Context.GetSelected())
		{
			glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);

			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();

			glm::mat4 transformMat = transform.GetTransform();

			// Local space square
			glm::vec4 p0 = transformMat * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
			glm::vec4 p1 = transformMat * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
			glm::vec4 p2 = transformMat * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
			glm::vec4 p3 = transformMat * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

			// Draw
			Renderer2D::DrawLine(glm::vec3(p0), glm::vec3(p1), color, selectedEntity);
			Renderer2D::DrawLine(glm::vec3(p1), glm::vec3(p2), color, selectedEntity);
			Renderer2D::DrawLine(glm::vec3(p2), glm::vec3(p3), color, selectedEntity);
			Renderer2D::DrawLine(glm::vec3(p3), glm::vec3(p0), color, selectedEntity);

			if (selectedEntity.HasComponent<CameraComponent>())
			{
				CameraComponent& camera = selectedEntity.GetComponent<CameraComponent>();

				glm::mat4 camTransform = transformMat;

				float aspect = camera.Camera.GetAspectRatio();

				if (camera.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float size = camera.Camera.GetOrthographicSize();
					float halfH = size * 0.5f;
					float halfW = halfH * aspect;

					float nearZ = -camera.Camera.GetOrthographicNearClip();
					float farZ = -camera.Camera.GetOrthographicFarClip();

					// Near
					glm::vec3 n0 = camTransform * glm::vec4(-halfW, -halfH, nearZ, 1.0f);
					glm::vec3 n1 = camTransform * glm::vec4(halfW, -halfH, nearZ, 1.0f);
					glm::vec3 n2 = camTransform * glm::vec4(halfW, halfH, nearZ, 1.0f);
					glm::vec3 n3 = camTransform * glm::vec4(-halfW, halfH, nearZ, 1.0f);

					// Far
					glm::vec3 f0 = camTransform * glm::vec4(-halfW, -halfH, farZ, 1.0f);
					glm::vec3 f1 = camTransform * glm::vec4(halfW, -halfH, farZ, 1.0f);
					glm::vec3 f2 = camTransform * glm::vec4(halfW, halfH, farZ, 1.0f);
					glm::vec3 f3 = camTransform * glm::vec4(-halfW, halfH, farZ, 1.0f);

					// Draw near
					Renderer2D::DrawLine(n0, n1, color, selectedEntity);
					Renderer2D::DrawLine(n1, n2, color, selectedEntity);
					Renderer2D::DrawLine(n2, n3, color, selectedEntity);
					Renderer2D::DrawLine(n3, n0, color, selectedEntity);

					// Draw far
					Renderer2D::DrawLine(f0, f1, color, selectedEntity);
					Renderer2D::DrawLine(f1, f2, color, selectedEntity);
					Renderer2D::DrawLine(f2, f3, color, selectedEntity);
					Renderer2D::DrawLine(f3, f0, color, selectedEntity);

					// Connect
					Renderer2D::DrawLine(n0, f0, color, selectedEntity);
					Renderer2D::DrawLine(n1, f1, color, selectedEntity);
					Renderer2D::DrawLine(n2, f2, color, selectedEntity);
					Renderer2D::DrawLine(n3, f3, color, selectedEntity);
				}
				else if (camera.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					glm::mat4 camTransform = transformMat;

					float aspect = camera.Camera.GetAspectRatio();
					float fov = camera.Camera.GetPerspectiveVerticalFOV();

					float nearZ = camera.Camera.GetPerspectiveNearClip();
					float farZ = camera.Camera.GetPerspectiveFarClip();

					// OpenGL convention camera looks along -Z
					nearZ = -nearZ;
					farZ = -farZ;

					float tanFov = tan(fov * 0.5f);

					float nearH = tanFov * abs(nearZ);
					float nearW = nearH * aspect;

					float farH = tanFov * abs(farZ);
					float farW = farH * aspect;

					// Near plane
					glm::vec3 n0 = camTransform * glm::vec4(-nearW, -nearH, nearZ, 1.0f);
					glm::vec3 n1 = camTransform * glm::vec4(nearW, -nearH, nearZ, 1.0f);
					glm::vec3 n2 = camTransform * glm::vec4(nearW, nearH, nearZ, 1.0f);
					glm::vec3 n3 = camTransform * glm::vec4(-nearW, nearH, nearZ, 1.0f);

					// Far plane
					glm::vec3 f0 = camTransform * glm::vec4(-farW, -farH, farZ, 1.0f);
					glm::vec3 f1 = camTransform * glm::vec4(farW, -farH, farZ, 1.0f);
					glm::vec3 f2 = camTransform * glm::vec4(farW, farH, farZ, 1.0f);
					glm::vec3 f3 = camTransform * glm::vec4(-farW, farH, farZ, 1.0f);

					// Draw near
					Renderer2D::DrawLine(n0, n1, color, selectedEntity);
					Renderer2D::DrawLine(n1, n2, color, selectedEntity);
					Renderer2D::DrawLine(n2, n3, color, selectedEntity);
					Renderer2D::DrawLine(n3, n0, color, selectedEntity);

					// Draw far
					Renderer2D::DrawLine(f0, f1, color, selectedEntity);
					Renderer2D::DrawLine(f1, f2, color, selectedEntity);
					Renderer2D::DrawLine(f2, f3, color, selectedEntity);
					Renderer2D::DrawLine(f3, f0, color, selectedEntity);

					// Connect
					Renderer2D::DrawLine(n0, f0, color, selectedEntity);
					Renderer2D::DrawLine(n1, f1, color, selectedEntity);
					Renderer2D::DrawLine(n2, f2, color, selectedEntity);
					Renderer2D::DrawLine(n3, f3, color, selectedEntity);
				}
			}
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
			std::filesystem::path dllPath = Project::GetProjectDirectory();
			dllPath /= "bin";
			dllPath /= "Game.dll";
			ScriptEngine::Update(dllPath);
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
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		m_Context.SetSelected({});
		OnSceneStop();

		std::string filename = path.filename().string();
		if (path.extension().string() != ".pol")
		{
			POL_WARN("Could not load %s - not a scene file '.pol'", filename.c_str());
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

			POL_INFO("Scene: %s opened!", filename.c_str());
		}
		else
		{
			POL_ERROR("Scene: %s failed to deserialize!", filename.c_str());
		}

	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorSceneFilepath.empty())
		{
			SceneSerializer serializer(m_Context.EditorScene);
			serializer.Serialize(m_EditorSceneFilepath.string());

			POL_INFO("Scene: %s saved!", m_EditorSceneFilepath.filename().string().c_str());
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
			POL_WARN("Scene not saved, has no filepath!");
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
		POL_TRACE("Scene Playing...");
		m_SceneState = SceneState::Play;

		m_Context.SetSelected({}); //TODO remember selected between play-edit
		m_Context.ActiveScene = Scene::Copy(m_Context.EditorScene);
		m_Context.ActiveScene->OnRuntimeStart();
	}

	void EditorLayer::OnSceneStop()
	{
		m_Context.ActiveScene->OnRuntimeStop();
		m_SceneState = SceneState::Edit;

		m_Context.SetSelected({});
		m_Context.ActiveScene = m_Context.EditorScene;

		POL_TRACE("Scene Stopped");
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
				ImGui::Separator();
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
				if (ImGui::MenuItem("Undo TODO", "Ctrl+Z"))
					Undo();
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
				if (ImGui::MenuItem("Reset Layout TODO"));
				if (ImGui::MenuItem("Save Layout TODO"));
				ImGui::Separator();

				bool editViewportOpen = m_PanelManager.GetPanel<ViewportPanel>() != nullptr;
				bool hierarchyOpen = m_PanelManager.GetPanel<HierarcyPanel>() != nullptr;
				bool assetsOpen = m_PanelManager.GetPanel<AssetsPanel>() != nullptr;
				bool consoleOpen = m_PanelManager.GetPanel<ConsolePanel>() != nullptr;

				if (ImGui::MenuItem("Edit Viewport", nullptr, editViewportOpen))
				{
					if (auto p = m_PanelManager.GetPanel<ViewportPanel>())
						p->Close();
					else
						m_PanelManager.OpenPanel<ViewportPanel>(m_Context);
				}
				if (ImGui::BeginMenu("Show"))
				{
					if (ImGui::MenuItem("Grid"))
					{

					}

					ImGui::Separator();
					if (ImGui::MenuItem("Gizmos"))
					{

					}

					ImGui::EndMenu();
				}
				ImGui::Separator();

				if (ImGui::MenuItem("Game Viewport TODO"));
				ImGui::Separator();

				if (ImGui::MenuItem("Hierarchy", nullptr, hierarchyOpen))
				{
					if (auto p = m_PanelManager.GetPanel<HierarcyPanel>())
						p->Close();
					else
						m_PanelManager.OpenPanel<HierarcyPanel>(m_Context);
				}
				ImGui::Separator();

				if (ImGui::MenuItem("Properties"))
					m_PanelManager.OpenPanel<PropertitiesPanel>(m_Context);

				ImGui::Separator();

				if (ImGui::MenuItem("Assets", nullptr, assetsOpen))
				{
					if (auto p = m_PanelManager.GetPanel<AssetsPanel>())
						p->Close();
					else
						m_PanelManager.OpenPanel<AssetsPanel>(m_Context);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Console", nullptr, consoleOpen))
				{
					if (auto p = m_PanelManager.GetPanel<ConsolePanel>())
					{
						p->Close();
					}
					else
					{
						m_PanelManager.OpenPanel<ConsolePanel>(m_Context);

						Logger::AddLogListener([this](const LogEvent& e)
						{
							if (auto* panel = m_PanelManager.GetPanel<ConsolePanel>())
								panel->BindLog(e);
						});
					}
				}

				ImGui::Separator();
				if (ImGui::BeginMenu("Themes"))
				{
					std::filesystem::path themePath = "assets/themes";
					if (std::filesystem::exists(themePath))
					{
						for (const auto& entry : std::filesystem::directory_iterator(themePath))
						{
							if (!entry.is_regular_file())
								continue;

							if (entry.path().extension() != ".txt")
								continue;

							std::string filename = entry.path().stem().string();

							if (ImGui::MenuItem(filename.c_str()))
							{
								EditorTheme::Load(entry.path());
							}
						}
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Open folder TODO"));
					ImGui::EndMenu();
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

	// WIP
	bool EditorLayer::IconMenuItem(const char* label, Ref<SubTexture2D> icon, bool selected)
	{
		float iconSize = 16.0f;

		uint32_t texID = icon ? icon->GetTexture()->GetRendererID() : 0;
		const glm::vec2* uvs = icon ? icon->GetTexCoords() : nullptr;

		ImGui::BeginGroup();

		if (icon)
		{
			ImGui::Image(
				(void*)texID,
				ImVec2(iconSize, iconSize),
				ImVec2(uvs[0].x, uvs[0].y),
				ImVec2(uvs[2].x, uvs[2].y)
			);
			ImGui::SameLine();
		}

		// text
		bool clicked = ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns);

		ImGui::EndGroup();

		return clicked;
	}
}
