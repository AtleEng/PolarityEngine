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
		: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f), m_viewportSize(1280, 720)
	{
	}

	void EditorLayer::OnAttach()
	{
		POLARITY_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_CamEntity = m_ActiveScene->Spawn("Main Camera");
		m_CamEntity.AddComponent<CameraComponent>();

		m_SCamEntity = m_ActiveScene->Spawn("PreRender Camera");
		auto& cam = m_SCamEntity.AddComponent<CameraComponent>();
		cam.FixedAspectRatio = true;

		auto square = m_ActiveScene->Spawn();
		auto& sprite = square.AddComponent<SpriteComponent>();
		sprite.Color = { 1, 0, 1, 1 };


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

		//------------ Resize window --------------------------------
		if (FramebufferSpecification spec = m_framebuffer->GetSpecification();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(spec.Width != m_viewportSize.x || spec.Height != m_viewportSize.y))
		{
			m_framebuffer->Resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_cameraController.OnResize(m_viewportSize.x, m_viewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		}

		if (m_ViewportFocused)
			m_cameraController.OnUpdate(tS);


		//------------ Render ---------------------------------------
		{
			POLARITY_PROFILE_SCOPE("Render Draw");

			Renderer2D::ResetStats();

			m_framebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			//------------ Scene -------------------------------------
			m_ActiveScene->OnUpdate(tS);

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

		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;

		style.WindowMenuButtonPosition = -1; // cant find ImGui enum (none)

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.86f, 0.70f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.84f, 0.77f, 0.63f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.36f, 0.33f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.49f, 0.44f, 0.39f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.57f, 0.51f, 0.45f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.66f, 0.60f, 0.52f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.41f, 0.62f, 0.42f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.27f, 0.52f, 0.53f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.74f, 0.68f, 0.58f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.98f, 0.29f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.84f, 0.60f, 0.13f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.84f, 0.36f, 0.05f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.19f, 0.18f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.98f, 0.95f, 0.78f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.52f, 0.53f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.98f, 0.74f, 0.18f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.27f, 0.52f, 0.53f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.98f, 0.95f, 0.78f, 1.00f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.84f, 0.77f, 0.63f, 1.00f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.74f, 0.68f, 0.58f, 1.00f);


		ImGui::SetNextWindowPos({ viewport->WorkPos.x,  viewport->WorkPos.y });
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
				float titleHeight = 24.0f; // TODO make a bigger title

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

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Drag area 
				float space = ImGui::GetContentRegionAvail().x - (32 * 3); // space minus buttons 

				ImGui::InvisibleButton("##DragZone", ImVec2(space, titleHeight));


				static bool dragging = false;
				static ImVec2 dragStartMouse;
				static glm::ivec2 dragStartWindow;

				if (ImGui::IsItemActive())
				{
					if (!dragging)
					{
						dragging = true;
						dragStartMouse = ImGui::GetMousePos();
						dragStartWindow = Application::Get().GetWindow().GetPosition();
					}

					ImVec2 mouseNow = ImGui::GetMousePos();
					ImVec2 delta = { mouseNow.x - dragStartMouse.x + 4, mouseNow.y - dragStartMouse.y + 31 };

					Application::Get().GetWindow().SetPosition({
						dragStartWindow.x + (int)delta.x,
						dragStartWindow.y + (int)delta.y
						});
				}
				else
				{
					dragging = false;
				}

				colors[ImGuiCol_Button] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
				// --- Window buttons --- 
				if (ImGui::Button("-", { 32, titleHeight }))
					LOG_DEBUG("Minimize!");
				if (ImGui::Button("[]", { 32, titleHeight }))
					LOG_DEBUG("Maximize!");
				if (ImGui::Button("X", { 32, titleHeight }))
					Application::Get().Shutdown();

				colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
				ImGui::PopStyleVar();

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
		glm::vec2 newSize = { viewportSize.x, viewportSize.y };
		if (m_viewportSize != newSize)
		{
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
			auto entity = m_ActiveScene->Spawn();
			entity.AddComponent<SpriteComponent>();
		}
		ImGui::SameLine();
		if (ImGui::Button("Kill"))
		{
			m_ActiveScene->Kill("NewEntity");
		}
		ImGui::SameLine();
		if (ImGui::Button("List"))
		{
			m_ActiveScene->List();
		}
		ImGui::Separator();
		
		for (auto entity : m_ActiveScene->GetView<NameComponent>())
		{
			NameComponent& name = entity.GetComponent<NameComponent>();
			if (ImGui::Button(name.Name.c_str()))
			{
				
			}
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

		Entity entity = m_CamEntity;

		ImGui::Begin("Inspector", nullptr);
		ImGui::Text(entity.GetName().c_str());
		ImGui::Separator();

		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;
			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::DragFloat2("Position", glm::value_ptr(transform[3]), 0.1f);
				ImGui::DragFloat2("Size", glm::value_ptr(transform[2]), 0.1f);
				ImGui::DragFloat("Rotation", glm::value_ptr(transform[1]), 0.1f);
			}
		}
		if (entity.HasComponent<CameraComponent>())
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::Checkbox("Main Camera", &camera.Primary);
				ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);
				ImGui::Text("AspectRatio: %f", camera.Camera.GetAspectRatio());
			}
		}
		if (entity.HasComponent<SpriteComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();
			if (ImGui::CollapsingHeader("Sprite"))
			{
				ImGui::InputText("Texture", textBuffer, IM_ARRAYSIZE(textBuffer));
				static float scale;
				ImGui::DragFloat("Scale", &sprite.Scale, 0.1f);
				static glm::vec4 color;
				ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.Color));
			}
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
