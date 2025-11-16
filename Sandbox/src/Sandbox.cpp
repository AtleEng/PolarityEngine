#include <Polarity.h>

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Polarity;

class TestLayer : public Polarity::Layer
{
public:
	TestLayer()
		: Layer("Test"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_camPos(0.0f), m_trianglePos(0.0f)
	{
		m_vertexArray.reset(VertexArray::Create());

		float vert[3 * 7] = {
			-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 1.0f, 1.0f,
			0.5f,  -0.5f, 0.0f,     0.0f, 1.0f, 1.0f, 1.0f,
			0.0f,  0.5f,  0.0f,     0.0f, 0.0f, 1.0f, 1.0f
		};
		uint32_t indices[3] = {
			0,1,2
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vert, sizeof(vert)));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
			});
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, 3));
		m_vertexArray->SetIndexBuffer(indexBuffer);


		std::string vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
	        gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
        )";
		std::string fragmentSource = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main()
        {
	        color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
        )";

		m_shader.reset(new Shader(vertexSource, fragmentSource));
	}

	void OnUpdate(Timestep dT) override
	{
		if (Input::IsKeyPressed(Key::W))
		{
			m_trianglePos.y += m_camSpeed * dT;
		}
		if (Input::IsKeyPressed(Key::A))
		{
			m_trianglePos.x -= m_camSpeed * dT;
		}
		if (Input::IsKeyPressed(Key::S))
		{
			m_trianglePos.y -= m_camSpeed * dT;
		}
		if (Input::IsKeyPressed(Key::D))
		{
			m_trianglePos.x += m_camSpeed * dT;
		}

		//cam
		if (Input::IsMouseButtonPressed(Mouse::Button0))
		{
			glm::vec2 mP = m_camera.ScreenToWorld(Input::GetMousePosition());
			LOG_DEBUG("Klicked at: [%f %f]", mP.x, mP.y);
		}
		if (Input::IsKeyPressed(Key::R))
		{
			m_camPos = glm::vec3(0.0f);
		}

		if (Input::IsMouseButtonPressed(Mouse::Button1))
		{
			glm::vec2 mP = m_camera.ScreenToWorld(Input::GetMousePosition());
			LOG_DEBUG("Klicked at: [%f %f]", mP.x, mP.y);

			difference = (glm::vec3(mP.x, mP.y, 0.0f) - m_camPos);
			if (drag == false)
			{
				drag = true;
				origin = glm::vec3(mP.x, mP.y, 0.0f);
			}
		}
		else
		{
			drag = false;
		}
		if (drag)
		{
			m_camPos = origin - difference;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_trianglePos);
		//------------ Render --------------------------------------

		m_camera.SetPosition({ m_camPos });
		m_camera.SetRotation(m_camRot);

		Renderer::BeginScene(m_camera);
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::Submit(m_shader, m_vertexArray, transform);

		Renderer::EndScene();
	}
	
	void OnImGuiRender() override
	{
	}
	
	void OnEvent(Polarity::Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(POLARITY_BIND_EVENT_FN(TestLayer::OnKeyPressedEvent));
	}

	bool OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::F5)
		{
			LOG_DEBUG("Debug mode?");
		}

		return false;
	}
	
private:
	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<VertexArray> m_vertexArray;

	OrthographicCamera m_camera;
	glm::vec3 m_camPos;
	float m_camSpeed = 1;

	float m_camRot = 0.0f;
	float m_camRotSpeed = 2;

	glm::vec3 m_trianglePos;

	glm::vec3 origin = glm::vec3(0.0f);
	glm::vec3 difference = glm::vec3(0.0f);
	glm::vec3 resetCameraPos = glm::vec3(0.0f);

	bool drag = false;

};

class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		LOG_INFO("\nStarting application...");

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