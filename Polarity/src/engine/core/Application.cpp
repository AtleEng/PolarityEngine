#include "polpch.h"
#include "Application.h"

#include "engine/renderer/Renderer.h"
#include "Input.h"

namespace Polarity {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_instance = nullptr;

    Application::Application()
        : m_camera(-1.6f, 1.6f, -0.9f, 0.9f)
    {
        LOG_ASSERT(!s_instance, "Application already exist !!!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        m_imGuiLayer = new ImGuiLayer();
        PushOverlay(m_imGuiLayer);



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

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
	        gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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
    //0.8, 0.2, 0.1, 1.0
    Application::~Application() = default;


    void Application::OnEvent(Event& e)
    {
        //LOG_TRACE("%s", e.ToString().c_str());

        EventDispatcher dispacher(e);
        dispacher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

        for (auto i = m_layerStack.end(); i != m_layerStack.begin();)
        {
            (*--i)->OnEvent(e);
            if (e.handled)
            {
                break;
            }
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_layerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Run()
    {
        while (m_running)
        {
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();

            m_camera.SetPosition({ 0.3f, 0.1f, 0.0f});
            m_camera.SetRotation(30);

            Renderer::BeginScene(m_camera);
            {
                Renderer::Submit(m_shader, m_vertexArray);

                Renderer::EndScene();
            }


            for (Layer* layer : m_layerStack)
            {
                layer->OnUpdate();
            }

            m_imGuiLayer->Begin();
            for (Layer* layer : m_layerStack)
            {
                layer->OnImGuiRender();
            }
            m_imGuiLayer->End();

            m_window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_running = false;

        return true;
    }
}
