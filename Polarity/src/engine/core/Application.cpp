#include "polpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

#include "glad/glad.h"

namespace Polarity {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_instance = nullptr;


    Application::Application()
    {
        LOG_ASSERT(!s_instance, "Application already exist !!!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        m_imGuiLayer = new ImGuiLayer();
        PushOverlay(m_imGuiLayer);

        glGenVertexArrays(1, &m_vertexArray);
        glBindVertexArray(m_vertexArray);

        glGenBuffers(1, &m_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

        float vert[3 * 3] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5, 0.0f,
            0.0f, 0.5f, 0.0f
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        glGenBuffers(1, &m_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

        unsigned int indices[3] = { 0,1,2 };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        std::string vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
	        gl_Position = vec4(a_Position, 1.0);
        }
        )";
        std::string fragmentSource = R"(
        #version 330 core

        layout(location = 0) out vec4 color;
        in vec3 v_Position;

        void main()
        {
	        color = vec4(v_Position * 0.5 + 0.5, 1.0);
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
            glClearColor(0.1f, 0.1f, 0.1f, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_shader->Bind();
            glBindVertexArray(m_vertexArray);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
