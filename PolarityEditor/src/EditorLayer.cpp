#include "EditorLayer.h"


#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

namespace Polarity
{
	struct EditorLog
	{
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.

		EditorLog()
		{
			AutoScroll = true;
			Clear();
		}

		void Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void Draw(const char* title, bool* p_open = NULL)
		{
			if (!ImGui::Begin(title, p_open))
			{
				ImGui::End();
				return;
			}

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);

			ImGui::Separator();

			if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				if (clear)
					Clear();
				if (copy)
					ImGui::LogToClipboard();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				const char* buf = Buf.begin();
				const char* buf_end = Buf.end();
				if (Filter.IsActive())
				{
					for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						if (Filter.PassFilter(line_start, line_end))
							ImGui::TextUnformatted(line_start, line_end);
					}
				}
				else
				{
					ImGuiListClipper clipper;
					clipper.Begin(LineOffsets.Size);
					while (clipper.Step())
					{
						for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
						{
							const char* line_start = buf + LineOffsets[line_no];
							const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
							ImGui::TextUnformatted(line_start, line_end);
						}
					}
					clipper.End();
				}
				ImGui::PopStyleVar();

				if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::End();
		}
	};

	static EditorLog g_EditorLog;

	void EditorLog_Listener(const LogEvent& e)
	{
		g_EditorLog.AddLog("%s %s %s\n",
			e.prefix.c_str(),
			e.time.c_str(),
			e.message.c_str());
	}

	EditorLayer::EditorLayer()
		: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f), m_viewportSize(1280, 720), m_Scene(CreateRef<Scene>())
	{
	}

	void EditorLayer::OnAttach()
	{
		POLARITY_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_framebuffer = Framebuffer::Create(fbSpec);

		AddLogListener(EditorLog_Listener);


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

		static bool showViewport = true;
		static bool showHierarcy = true;
		static bool showConsole = true;
		static bool showInspector = true;
		static bool showAssets = true;
		static bool showProfiler = false;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | 
			ImGuiWindowFlags_MenuBar;

		style.WindowMenuButtonPosition = -1; // cant find ImGui enum (none)
		style.FrameRounding = 2;
		style.GrabRounding = 5;
		style.FrameBorderSize = 1;
		style.ChildBorderSize = 0;
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.23f, 0.23f, 0.23f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.34f, 0.34f, 0.34f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.34f, 0.34f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.55f, 0.33f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.21f, 0.28f, 0.38f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.37f, 0.50f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.30f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.29f, 0.35f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.21f, 0.24f, 0.27f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.18f, 0.24f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.19f, 0.21f, 0.23f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.12f, 0.12f, 0.14f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.32f, 0.34f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.28f, 0.34f, 0.40f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.08f, 0.08f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.17f, 0.19f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.11f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.12f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.34f, 0.36f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);



		ImGui::SetNextWindowPos({ viewport->WorkPos.x,  viewport->WorkPos.y});
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 1.0f);
		if (ImGui::Begin("DockSpace", nullptr, window_flags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 8.0f));
			if (ImGui::BeginMenuBar())
			{
				float titleHeight = 24.0f;

				uint32_t texID = m_logoTex->GetRendererID();
				ImGui::Image((void*)texID, ImVec2{ titleHeight, titleHeight },
					ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene TODO"));
					if (ImGui::MenuItem("Open... TODO"));
					if (ImGui::MenuItem("Build Game TODO"));
					ImGui::Separator();
					if (ImGui::MenuItem("Quit")) Application::Get().Shutdown();

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
					if (ImGui::MenuItem("Reset Layout")); //TODO

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

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleVar(2);


			// Submit the DockSpace
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			ImGui::ShowDemoWindow();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 2.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
			if (showViewport) { ShowViewport(); }
			if (showHierarcy) { ShowHierarcy(); }
			if (showConsole) { ShowConsole(); }
			if (showInspector) { ShowInspector(); }
			if (showAssets) { ShowAssets(); }
			if (showProfiler) { ShowProfiler(); }
			
			ImGui::PopStyleVar(2);

			ImGui::End();
		}
		ImGui::PopStyleVar(4);
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

	void EditorLayer::ShowViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
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

	void EditorLayer::ShowHierarcy()
	{
		ImGui::Begin("Hierarcy", nullptr);

		if (ImGui::Button("Spawn"))
		{
			m_Scene->Spawn();
		}
		if (ImGui::Button("Kill"))
		{
			m_Scene->Kill(0); // WARNING
		}
		if (ImGui::Button("List"))
		{
			m_Scene->List();
		}

		ImGui::End();
	}

	void EditorLayer::ShowConsole()
	{
		g_EditorLog.Draw("Console");
	}

	void EditorLayer::ShowInspector()
	{
		static char textBuffer[256] = "";
		static glm::vec2 pos;

		ImGui::Begin("Inspector", nullptr);
		ImGui::Text("Name of entity");
		ImGui::Separator();
		if (ImGui::BeginTabBar("Components"))
		{
			if (ImGui::BeginTabItem("Transform"))
			{
			ImGui::DragFloat2("Position", glm::value_ptr(pos), 0.1f);
			ImGui::DragFloat2("Size", glm::value_ptr(pos), 0.1f);
			ImGui::DragFloat("Rotation", glm::value_ptr(pos), 0.1f);
			ImGui::Separator();
			ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Audio"))
			{
			ImGui::InputText("##TextInput", textBuffer, IM_ARRAYSIZE(textBuffer));
			ImGui::SliderFloat("Volume", glm::value_ptr(pos), 0.00f, 1.00f);
			ImGui::Separator();
			ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Sprite"))
			{
				ImGui::InputText("Texture", textBuffer, IM_ARRAYSIZE(textBuffer));
				static float scale;
				ImGui::DragFloat("Scale", &scale, 0.1f);
				static glm::vec4 color;
				ImGui::ColorEdit4("Tint", glm::value_ptr(color));

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void EditorLayer::ShowAssets()
	{
		ImGui::Begin("Assets", nullptr);

		ImGui::End();
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
