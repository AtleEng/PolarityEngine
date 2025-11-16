#include <Polarity.h>

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platform/openGL/OpenGLShader.h" //temp

using namespace Polarity;

class TestLayer : public Polarity::Layer
{
public:
	TestLayer()
		: Layer("Test"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_camPos(0.0f), m_trianglePos(0.0f)
	{
		//triangle
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

		//square
		float squareV[4 * 7] = {
		-0.5f, -0.5f, 0.0f, 	 1.0f,1.0f,1.0f,1.0f,
		 0.5f, -0.5f, 0.0f, 	 1.0f,1.0f,1.0f,1.0f,
		 0.5f,  0.5f, 0.0f, 	 1.0f,1.0f,1.0f,1.0f,
		-0.5f,  0.5f, 0.0f, 	 1.0f,1.0f,1.0f,1.0f
		};
		uint32_t squareI[6] = { 0, 1, 2, 2, 3, 0 };


		m_squareVA.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareV, sizeof(squareV)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
			});
		m_squareVA->AddVertexBuffer(squareVB);

		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareI, sizeof(squareI)));
		m_squareVA->SetIndexBuffer(squareIB);


		std::string vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;


        void main()
        {
	        gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
        )";
		std::string fragmentSource = R"(
        #version 330 core

        layout(location = 0) out vec3 color;

        uniform vec3 u_Color;

        void main()
        {
            color = u_Color;
        }
        )";

		m_shader.reset(Shader::Create(vertexSource, fragmentSource));
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

		if (Input::IsMouseButtonPressed(Mouse::Button2))
		{
			glm::vec2 mP = m_camera.ScreenToWorld(Input::GetMousePosition());

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

		/* TODO
		MaterialRef concreteMaterial = new Material(m_shader);
		MaterialInstance floorMaterial = new MaterialInstance(concreteMaterial);

		floorMaterial->SetValue("u_Color", m_color);
		floorMaterial->SetTexture("u_textureMap", texture);

		squareMesh->SetMaterial(floorMaterial);
		*/

		Renderer::Submit(m_shader, m_vertexArray, transform);

		std::dynamic_pointer_cast<OpenGLShader>(m_shader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_shader)->UploadUniformFloat3("u_Color", m_color);

		for (int y = 5; y < 10; y++)
		{
			for (int x = 5; x < 10; x++)
			{
				glm::mat4 scale =  glm::scale(glm::mat4(1.0f), glm::vec3(0.08f));
				glm::mat4 trans = glm::translate(glm::mat4(1.0f), {((float)x) / 10, ((float)y) / 10, 0}) * scale;

				Renderer::Submit(m_shader, m_squareVA, trans);
			}
		}

		Renderer::EndScene();
	}
	
	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: ");
		ImGui::Text("Quads: ");
		ImGui::Text("Vertices: ");
		ImGui::Text("Indices: ");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_color));

		ImGui::End();
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

	std::shared_ptr<VertexArray> m_squareVA;
	glm::vec3 m_color = glm::vec3(0.8f, 0.2f, 0.3f);

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