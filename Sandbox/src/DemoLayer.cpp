#include "DemoLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



DemoLayer::DemoLayer()
	: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f)
{
}

void DemoLayer::OnAttach()
{
	/*
	auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
	m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");
	*/

	m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");

}

void DemoLayer::OnDetach()
{
	
}

void DemoLayer::OnUpdate(Timestep tS)
{
	m_cameraController.OnUpdate(tS);


	//------------ Render --------------------------------------
	Renderer2D::BeginScene(m_cameraController.GetCamera());


	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	

	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			float fx = (float)x;
			float fy = (float)y;

			// --- Position (spread out)
			glm::vec2 pos = { fx * 3.0f, fy * 3.0f };

			// --- Wavy size pattern
			glm::vec2 size = {
				1.0f + 0.5f * sin(fx + fy * 0.3f),
				1.0f + 0.5f * cos(fx * 0.2f + fy)
			};

			// --- Smooth rotation spiral
			float rotation = (fx * 15.0f) + (fy * 8.0f);

			// --- Beautiful rainbow gradient
			glm::vec4 color = {
				(sin(fx * 0.5f) * 0.5f) + 0.5f,
				(sin(fy * 0.5f) * 0.5f) + 0.5f,
				(sin((fx + fy) * 0.3f) * 0.5f) + 0.5f,
				1.0f
			};

			Renderer2D::DrawQuad(pos, size, rotation, color);
		}
		Renderer2D::DrawQuad(m_logoTex, m_pos, m_size, m_rotation, m_color);
	}


	Renderer2D::EndScene();
	
}

void DemoLayer::OnImGuiRender()
{
	if (isDebugging)
	{
		ImGui::Begin("Settings");

		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: ");
		ImGui::Text("Quads: ");
		ImGui::Text("Vertices: ");
		ImGui::Text("Indices: ");

		ImGui::Text("\nQuad:");
		ImGui::Text("Position");
		ImGui::DragFloat3("##pos", glm::value_ptr(m_pos), 0.1f);
		ImGui::Text("Size");
		ImGui::DragFloat2("##size", glm::value_ptr(m_size), 0.1f);
		ImGui::Text("Rotation");
		ImGui::DragFloat("##rotation", &m_rotation, 1.0f);
		ImGui::Text("Color");
		ImGui::ColorEdit4("##color", glm::value_ptr(m_color));

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
