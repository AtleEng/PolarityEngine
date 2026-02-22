#include "DemoLayer.h"
#include "Grid.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

DemoLayer::DemoLayer()
	: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f), m_particleSystem()
{
}

void DemoLayer::OnAttach()
{
	POL_PROFILE_FUNCTION();

	Random::Init();
	m_particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_particle.SizeBegin = 0.5f, m_particle.SizeVariation = 0.3f, m_particle.SizeEnd = 0.0f;
	m_particle.LifeTime = 1.0f;
	m_particle.Velocity = { 0.0f, 0.0f };
	m_particle.VelocityVariation = { 1.0f, 1.0f };
	m_particle.Position = { -3.0f, 0.0f };

	/*
	auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
	m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");
	*/

	m_atlasTex = Texture2D::Create("assets/textures/tileAtlas.png");
	m_spritemap = {
		SubTexture2D::Create(m_atlasTex, { 16, 80 }, { 16, 16 }),
		SubTexture2D::Create(m_atlasTex, { 32, 80 }, { 16, 16 }),
		SubTexture2D::Create(m_atlasTex, { 0,  64 }, { 16, 16 }),
		SubTexture2D::Create(m_atlasTex, { 16, 64 }, { 16, 16 }),
		SubTexture2D::Create(m_atlasTex, { 32, 64 }, { 16, 16 })
	};
		
	m_gridTex = Texture2D::Create("assets/textures/grid.png");

	m_clickSound = Audio::Create("assets/audio/click.wav");
}

void DemoLayer::OnDetach()
{

}

void DemoLayer::OnUpdate(Timestep tS)
{
	POL_PROFILE_FUNCTION();

	Renderer2D::ResetStats();

	m_cameraController.OnUpdate(tS);
	m_particleSystem.OnUpdate(tS);

	for (int i = 0; i < 1; i++)
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

		int n = 5;

		for (int x = 0; x < n; x++)
		{
			for (int y = 0; y < n; y++)
			{
				glm::vec2 pos = { (float)x , (float)y };


				Renderer2D::DrawQuad(m_spritemap[x], pos);
			}
		}

		//Renderer2D::DrawQuad(m_pos, m_size, m_rotation, m_color);

		m_particleSystem.OnRender();

		Renderer2D::EndScene();
	}
}

void DemoLayer::OnImGuiRender()
{
	POL_PROFILE_FUNCTION();

	if (ImGui::Begin("Settings"))
	{
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

		if (ImGui::Button("Quit Game"))
		{
			Application::Get().Shutdown();
		}
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

		
		//Audio::Stop(m_clickSound);
		Audio::Play(m_clickSound);	
	}
	return false;
}
