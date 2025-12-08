#include "EditorLayer.h"


#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

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
		m_framebuffer = Framebuffer::Create(fbSpec);


		m_atlasTex = Texture2D::Create("assets/textures/tileAtlas.png");
		m_spritemap = {
			SubTexture2D::Create(m_atlasTex, { 16, 80 }, { 16, 16 }),
			SubTexture2D::Create(m_atlasTex, { 32, 80 }, { 16, 16 }),
			SubTexture2D::Create(m_atlasTex, { 0,  64 }, { 16, 16 }),
			SubTexture2D::Create(m_atlasTex, { 16, 64 }, { 16, 16 }),
			SubTexture2D::Create(m_atlasTex, { 32, 64 }, { 16, 16 })
		};

		m_gridTex = Texture2D::Create("assets/textures/grid.png");
		m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");

		m_clickSound = Audio::Create("assets/audio/click.wav");
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep tS)
	{
		POLARITY_PROFILE_FUNCTION();

		Renderer2D::ResetStats();
		if (m_ViewportFocused)
			m_cameraController.OnUpdate(tS);

		//------------ Render --------------------------------------
		{
			POLARITY_PROFILE_SCOPE("Render Draw");

			Renderer2D::BeginScene(m_cameraController.GetCamera());

			{
				POLARITY_PROFILE_SCOPE("RenderPrep");
				m_framebuffer->Bind();
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
				RenderCommand::Clear();
			}

			Renderer2D::DrawQuad(m_gridTex, { 0.0f, 0.0f , -0.1f }, { 100.0f, 100.0f }, 0, { 0.1f, 0.1f, 0.1f, 1.0f }, 50);

			int n = 5;

			for (int x = 0; x < n; x++)
			{
				for (int y = 0; y < n; y++)
				{
					glm::vec2 pos = { (float)x , (float)y };


					Renderer2D::DrawQuad(m_spritemap[x], pos);
				}
			}


			Renderer2D::EndScene();
			m_framebuffer->Unbind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		POLARITY_PROFILE_FUNCTION();

		static float volume = 1;

		static bool showViewport = true;
		static bool showHierarcy = true;
		static bool showConsole = true;
		static bool showInspector = true;
		static bool showAssets = true;
		static bool showProfiler = false;

		static bool enable = true;
		ImGui::ShowDemoWindow(&enable);

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &enable, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			uint32_t texID = m_logoTex->GetRendererID();
			ImGui::Image((void*)texID, ImVec2{ 20, 20 },
				ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New TODO"));
				if (ImGui::MenuItem("Open TODO"));
				if (ImGui::MenuItem("Build TODO"));
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) Application::Get().Shutdown();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Project Settings TODO"));
				if (ImGui::MenuItem("Editor Settings TODO"));

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Viewport", NULL, &showViewport));
				if (ImGui::MenuItem("Hierarcy", NULL, &showHierarcy));
				if (ImGui::MenuItem("Console", NULL, &showConsole));
				if (ImGui::MenuItem("Inspector", NULL, &showInspector));
				if (ImGui::MenuItem("Assets", NULL, &showAssets));
				if (ImGui::MenuItem("Profiler", NULL, &showProfiler));
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Layout")); //Layout...

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("?"));

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("?"));
				if (ImGui::MenuItem("?"));
				if (ImGui::MenuItem("?"));
				if (ImGui::MenuItem("?"));
				if (ImGui::MenuItem("?"));

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (showViewport)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

			ImGui::Begin("Viewport");
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer().BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			if (m_viewportSize != *(glm::vec2*)&viewportSize)
			{
				m_framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
				m_viewportSize = { viewportSize.x, viewportSize.y };

				m_cameraController.OnResize(viewportSize.x, viewportSize.y);
			}
			uint32_t textureID = m_framebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y },
				{ 0,1 }, { 1,0 });

			ImGui::End();
			ImGui::PopStyleVar();
		}
		if (showHierarcy)
		{
			ImGui::Begin("Hierarcy");

			ImGui::End();
		}
		if (showConsole)
		{
			ImGui::Begin("Console");

			ImGui::DragFloat("Volume", &volume, 0.01f, 0.00f, 1.00f);
			Audio::SetMasterVolume(volume);

			ImGui::End();
		}
		if (showInspector)
		{
			static char textBuffer[256] = "";

			ImGui::Begin("Inspector");
			ImGui::Text("Name of entity");
			ImGui::Separator();
			ImGui::InputText("##TextInput", textBuffer, IM_ARRAYSIZE(textBuffer));

			ImGui::End();
		}
		if (showAssets)
		{
			ImGui::Begin("Assets");

			ImGui::End();
		}

		if (showProfiler)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags =
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav |
				ImGuiWindowFlags_NoMove;

			const float PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImVec2 window_pos = ImVec2(
				viewport->WorkPos.x + viewport->WorkSize.x - PAD,
				viewport->WorkPos.y + PAD
			);

			ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);

			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::SetNextWindowBgAlpha(0.45f);

			ImGui::SetNextWindowSize(ImVec2(160.0f, 0.0f));

			ImGui::Begin("Performance", nullptr, window_flags);

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

		ImGui::End();
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
			//Audio::Stop(m_clickSound);
			Audio::Play(m_clickSound);
		}
		return false;
	}

}
