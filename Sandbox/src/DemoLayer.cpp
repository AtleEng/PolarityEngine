#include "DemoLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platform/openGL/OpenGLShader.h" //temp

DemoLayer::DemoLayer()
	: Layer("DemoLayer"), m_cameraController(1280.0f / 720.0f)
{
}

void DemoLayer::OnAttach()
{
	//m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

	//square
	float squareV[5 * 4] = {
		//	vertex		  		 // UV
		-0.5f, -0.5f, 0.0f, 	 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 	 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 	 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 	 0.0f, 1.0f
	};
	uint32_t squareI[6] = { 0, 1, 2, 2, 3, 0 };

	m_squareVA = VertexArray::Create();

	Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareV, sizeof(squareV));
	squareVB->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord"    }
		});
	m_squareVA->AddVertexBuffer(squareVB);

	Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareI, sizeof(squareI));
	m_squareVA->SetIndexBuffer(squareIB);



	auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
	m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");

	m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");

	std::dynamic_pointer_cast<OpenGLShader>(textureShader)->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
}

void DemoLayer::OnDetach()
{

}

void DemoLayer::OnUpdate(Timestep tS)
{
	m_cameraController.OnUpdate(tS);

	//------------ Render --------------------------------------

	

	Renderer::BeginScene(m_cameraController.GetCamera());

	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();


	auto flatColorShader = m_shaderLibrary.Get("FlatColor");

	std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->UploadUniformFloat4("u_Color", m_color);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), { 2, 0, 0 }) * scale;

			Renderer::Submit(flatColorShader, m_squareVA, trans);

	auto textureShader = m_shaderLibrary.Get("Texture");
	m_logoTex->Bind();
	Renderer::Submit(textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

	Renderer::EndScene();
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
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_color));

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
