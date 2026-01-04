#include "EditorLayer.h"


#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Panels/AssetsPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/HierarcyPanel.h"

#include "Panels/PanelManager.h"

#include "engine/scene/SceneSerializer.h"

namespace Polarity
{
	EditorLayer::EditorLayer()
		: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		POLARITY_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_EditorContext.ViewportFramebuffer = Framebuffer::Create(fbSpec);
		m_EditorContext.ActiveScene = CreateRef<Scene>();

		// --------------------------------------------------- Panels
		PanelManager::Init();
		PanelManager::AddPanel<AssetsPanel>();
		PanelManager::AddPanel<HierarcyPanel>();
		PanelManager::AddPanel<InspectorPanel>();
		PanelManager::AddPanel<MenubarPanel>();
		PanelManager::AddPanel<ViewportPanel>();
		auto consolePanel = PanelManager::AddPanel<ConsolePanel>();
		std::weak_ptr<ConsolePanel> weakPanel = consolePanel;

		AddLogListener([weakPanel](const LogEvent& e)
		{
			if (auto panel = weakPanel.lock())
			{
				panel->Bind_Log(e);
			}
		});
		PanelManager::SetContext(m_EditorContext);
		//-------------------------------------------------------- Entites
		m_CamEntity = m_EditorContext.ActiveScene->Spawn("Main Camera");
		m_CamEntity.AddComponent<CameraComponent>();

		m_SCamEntity = m_EditorContext.ActiveScene->Spawn("PreRender Camera");
		auto& cam = m_SCamEntity.AddComponent<CameraComponent>();
		cam.FixedAspectRatio = true;

		auto square = m_EditorContext.ActiveScene->Spawn();
		auto& sprite = square.AddComponent<SpriteComponent>();
		sprite.Color = { 1, 0, 1, 1 };

		m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");

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

		PanelManager::GetPanel<ViewportPanel>()->UpdateViewport();

		if (Input::IsKeyPressed(Key::Delete) && m_EditorContext.SelectedEntity.IsAlive())
		{
			m_EditorContext.ActiveScene->Kill(m_EditorContext.SelectedEntity.GetID());
		}


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
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;

		style.WindowMenuButtonPosition = -1; // cant find ImGui enum (none)

		ImVec4* colors = ImGui::GetStyle().Colors;
		{
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


		}

		ImGui::SetNextWindowPos({ viewport->WorkPos.x,  viewport->WorkPos.y });
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 1.0f);
		if (ImGui::Begin("DockSpace", nullptr, window_flags))
		{
			// Submit the DockSpace
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			ImGui::PopStyleVar();
			ImGui::ShowDemoWindow();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 6.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 2.0f));
			
			PanelManager::OnDraw();

			ImGui::PopStyleVar(4);

			ImGui::End();
		}
		
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));
		m_cameraController.OnEvent(event);
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::F3)
		{
			
		}
		return false;
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
}
