#include "DemoLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

DemoLayer::DemoLayer()
	: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f), m_particleSystem()
{
}

void DemoLayer::OnAttach()
{
	Random::Init();
	m_particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_particle.SizeBegin = 0.5f, m_particle.SizeVariation = 0.3f, m_particle.SizeEnd = 0.0f;
	m_particle.LifeTime = 1.0f;
	m_particle.Velocity = { 0.0f, 0.0f };
	m_particle.VelocityVariation = { 3.0f, 1.0f };
	m_particle.Position = { 0.0f, 0.0f };

	/*
	auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
	m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");
	*/

	m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");
	m_gridTex = Texture2D::Create("assets/textures/grid.png");

}

void DemoLayer::OnDetach()
{

}

void DemoLayer::OnUpdate(Timestep tS)
{
	POLARITY_PROFILE_FUNCTION();

	Renderer2D::ResetStats();

	m_cameraController.OnUpdate(tS);
	m_particleSystem.OnUpdate(tS);

	for (int i = 0; i < 5; i++)
		m_particleSystem.Emit(m_particle);

	//------------ Render --------------------------------------
	{
		POLARITY_PROFILE_SCOPE("Render Draw");

		Renderer2D::BeginScene(m_cameraController.GetCamera());

		{
			POLARITY_PROFILE_SCOPE("RenderPrep");
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();
		}

		Renderer2D::DrawQuad(m_gridTex, { 0.0f, 0.0f , -0.1f }, { 100.0f, 100.0f }, 0, { 0.1f, 0.1f, 0.1f, 1.0f }, 50);

		int n = 0;

		for (int x = 0; x < n; x++)
		{
			for (int y = 0; y < n; y++)
			{
				float fx = (float)x;
				float fy = (float)y;

				// --- Position (spread out)
				glm::vec3 pos = { fx * 1.1f, fy * 1.1f, fx / n };

				// --- Wavy size pattern
				glm::vec2 size = {
					1.0f + 0.5f * sin(fx + fy * 0.3f),
					1.0f + 0.5f * cos(fx * 0.2f + fy)
				};

				// --- Smooth rotation spiral
				float rotation = (fx * 15.0f) + (fy * 8.0f);

				// --- "Beautiful" rainbow gradient
				glm::vec4 color = {
					(sin(fx * 0.5f) * 0.5f) + 0.5f,
					(sin(fy * 0.5f) * 0.5f) + 0.5f,
					(sin((fx + fy) * 0.3f) * 0.5f) + 0.5f,
					1.0f
				};

				Renderer2D::DrawQuad(m_logoTex, pos, size, rotation, color);
			}
		}

		//Renderer2D::DrawQuad(m_pos, m_size, m_rotation, m_color);

		m_particleSystem.OnRender();

		Renderer2D::EndScene();
	}
}

void DemoLayer::OnImGuiRender()
{
	POLARITY_PROFILE_FUNCTION();


	ImGui::Begin("Settings");

	ImGui::Text("Particle System:");
	ImGui::DragFloat("LifeTime", &m_particle.LifeTime, 0.01f);
	ImGui::Separator();

	ImGui::Text("Size:");
	ImGui::DragFloat("Start size", &m_particle.SizeBegin, 0.01f);
	ImGui::DragFloat("Variation", &m_particle.SizeVariation, 0.01f);
	ImGui::DragFloat("End size", &m_particle.SizeEnd, 0.01f);
	ImGui::Separator();

	ImGui::Text("Velocity");
	ImGui::DragFloat2("Start velocity", glm::value_ptr(m_particle.Velocity), 0.01f);
	ImGui::DragFloat2("Varitation", glm::value_ptr(m_particle.VelocityVariation), 0.01f);
	ImGui::Separator();

	ImGui::Text("Color");
	ImGui::ColorEdit4("Start color", glm::value_ptr(m_particle.ColorBegin));
	ImGui::ColorEdit4("End color", glm::value_ptr(m_particle.ColorEnd));


	ImGui::End();

	if (isDebugging)
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
}

void DemoLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));

	m_cameraController.OnEvent(event);
}

bool DemoLayer::OnKeyPressedEvent(KeyPressedEvent& event)
{
	if (event.GetKeyCode() == Key::F3)
	{
		isDebugging = !isDebugging;
	}
	return false;
}
