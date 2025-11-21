#include "DemoLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>



DemoLayer::DemoLayer()
	: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f)
{
}

void DemoLayer::OnAttach()
{
	//m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

	auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
	m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");

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

	Renderer2D::DrawQuad(m_pos, m_size, m_rotation, m_color);


	Renderer2D::EndScene();
	/*
	auto flatColorShader = m_shaderLibrary.Get("FlatColor");

	std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->UploadUniformFloat4("u_Color", m_color);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), { 2, 0, 0 }) * scale;

			Renderer::Submit(flatColorShader, m_squareVA, trans);

	auto textureShader = m_shaderLibrary.Get("Texture");
	m_logoTex->Bind();
	Renderer::Submit(textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));
	*/
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
	if (event.GetKeyCode() == Key::F5)
	{
		isDebugging = !isDebugging;
	}
	return false;
}
