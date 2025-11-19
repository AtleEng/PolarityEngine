#include <Polarity.h>

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platform/openGL/OpenGLShader.h" //temp

using namespace Polarity;

class TestLayer : public Layer
{
public:
	TestLayer()
		: Layer("Test"), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_camPos(0.0f)
	{
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

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

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareV, sizeof(squareV));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord"    }
			});
		m_squareVA->AddVertexBuffer(squareVB);

		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareI, sizeof(squareI));
		m_squareVA->SetIndexBuffer(squareIB);



		auto textureShader = m_shaderLibrary.Load("assets/shaders/Texture.glsl");
		m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");

		m_groundTex = Texture2D::Create("assets/textures/Ground.png");
		m_logoTex = Texture2D::Create("assets/textures/PolarityLogo.png");

		std::dynamic_pointer_cast<OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Timestep dT) override
	{
		//cam

		if (Input::IsKeyPressed(Key::R))
		{
			m_camPos = glm::vec3(0.0f);
			m_zoomLevel = 1;
			m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		}

		if (Input::IsMouseButtonPressed(Mouse::Button2))
		{
			glm::vec2 mP = m_camera.ScreenToWorld(Input::GetMousePosition());

			difference = (glm::vec3(mP.x, mP.y, 0.0f) - m_camPos);
			if (isPaning == false)
			{
				isPaning = true;
				origin = glm::vec3(mP.x, mP.y, 0.0f);
			}
		}
		else
		{
			isPaning = false;
		}
		if (isPaning)
		{
			m_camPos = origin - difference;
		}

		//------------ Render --------------------------------------

		m_camera.SetPosition({ m_camPos });

		Renderer::BeginScene(m_camera);

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		
		auto flatColorShader = m_shaderLibrary.Get("FlatColor");

		std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(flatColorShader)->UploadUniformFloat4("u_Color", m_color);

		for (int y = -5; y < 5; y++)
		{
			for (int x = 7; x < 17; x++)
			{
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.08f));
				glm::mat4 trans = glm::translate(glm::mat4(1.0f), { ((float)x + 0.52f) / 10, ((float)y + 0.52f) / 10, 0 }) * scale;

				Renderer::Submit(flatColorShader, m_squareVA, trans);
			}
		}

		auto textureShader = m_shaderLibrary.Get("Texture");
		m_groundTex->Bind();
		Renderer::Submit(textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

		m_logoTex->Bind();
		Renderer::Submit(textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

		Renderer::EndScene();
	}

	void OnImGuiRender() override
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

	void OnEvent(Polarity::Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>	(POLARITY_BIND_EVENT_FN(OnKeyPressedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>	(POLARITY_BIND_EVENT_FN(OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>	(POLARITY_BIND_EVENT_FN(OnWindowResized));
	}

	bool OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::F5)
		{
			isDebugging = !isDebugging;
		}
		return false;
	}
	bool OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_zoomLevel -= e.GetYOffset() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}
	void OnResize(float width, float height)
	{
		m_aspectRatio = width / height;
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}
	bool OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

private:
	ShaderLibrary m_shaderLibrary;

	Ref<VertexArray> m_squareVA;
	glm::vec4 m_color = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);

	Ref<Texture2D> m_groundTex, m_logoTex;

	OrthographicCamera m_camera;
	glm::vec3 m_camPos;

	float m_aspectRatio = 1280.0f/720.0f;
	float m_zoomLevel = 1.0f;

	glm::vec3 origin = glm::vec3(0.0f);
	glm::vec3 difference = glm::vec3(0.0f);
	glm::vec3 resetCameraPos = glm::vec3(0.0f);

	bool isPaning = false;

	bool isDebugging = false;
};

class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		LOG_INFO("Starting application ...\n");

		PushLayer(new TestLayer());
	}
	~Sandbox()
	{

	}

};

Polarity::Application* Polarity::CreateApplication()
{
	return new Sandbox();
}