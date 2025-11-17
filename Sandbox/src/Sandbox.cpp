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
		: Layer("Test"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_camPos(0.0f)
	{
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

		//Shader things [temporary]
		{

		std::string flatColorVertSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;


        void main()
        {
	        gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
        )";
		std::string flatColorFragSrc = R"(
        #version 330 core

        layout(location = 0) out vec3 color;

        uniform vec3 u_Color;

        void main()
        {
            color = u_Color;
        }
        )";

		m_flatColorShader = Shader::Create(flatColorVertSrc, flatColorFragSrc);


		std::string textureVertSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec2 a_TexCoord;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
		
		out vec2 v_TexCoord;

        void main()
        {
			v_TexCoord = a_TexCoord;
	        gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
        )";

		std::string textureFragSrc = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

		in vec2 v_TexCoord;

        uniform sampler2D u_Texture;

        void main()
        {
            color = texture(u_Texture, v_TexCoord);
        }
        )";

		m_textureShader = Shader::Create(textureVertSrc, textureFragSrc);
		}
		
		m_groundTex = Texture2D::Create("assets/textures/Ground.png");

		std::dynamic_pointer_cast<OpenGLShader>(m_textureShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Timestep dT) override
	{
		//cam
		
		if (Input::IsKeyPressed(Key::R))
		{
			m_camPos = glm::vec3(0.0f);
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

		/* TODO
		MaterialRef concreteMaterial = new Material(m_shader);
		MaterialInstance floorMaterial = new MaterialInstance(concreteMaterial);

		floorMaterial->SetValue("u_Color", m_color);
		floorMaterial->SetTexture("u_textureMap", texture);

		squareMesh->SetMaterial(floorMaterial);
		*/


		std::dynamic_pointer_cast<OpenGLShader>(m_flatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_flatColorShader)->UploadUniformFloat3("u_Color", m_color);

		for (int y = -5; y < 5; y++)
		{
			for (int x = 7; x < 17; x++)
			{
				glm::mat4 scale =  glm::scale(glm::mat4(1.0f), glm::vec3(0.08f));
				glm::mat4 trans = glm::translate(glm::mat4(1.0f), {((float)x + 0.52f) / 10, ((float)y + 0.52f) / 10, 0}) * scale;

				Renderer::Submit(m_flatColorShader, m_squareVA, trans);
			}
		}
		m_groundTex->Bind();
		Renderer::Submit(m_textureShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

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
			ImGui::ColorEdit3("Square Color", glm::value_ptr(m_color));

			ImGui::End();
		}
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
			isDebugging = !isDebugging;
		}
		return false;
	}
	
private:
	Ref<Shader> m_flatColorShader, m_textureShader;

	Ref<VertexArray> m_squareVA;
	glm::vec3 m_color = glm::vec3(0.8f, 0.2f, 0.3f);

	Ref<Texture2D> m_groundTex;

	OrthographicCamera m_camera;
	glm::vec3 m_camPos;

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
		LOG_INFO("Starting application...");

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