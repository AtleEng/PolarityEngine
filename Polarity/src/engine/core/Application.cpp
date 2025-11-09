#include "polpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

#include "glad/glad.h"

namespace Polarity {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_instance = nullptr;


    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case Polarity::ShaderDataType::Float:	return GL_FLOAT;
        case Polarity::ShaderDataType::Float2:	return GL_FLOAT;
        case Polarity::ShaderDataType::Float3:	return GL_FLOAT;
        case Polarity::ShaderDataType::Float4:	return GL_FLOAT;
        case Polarity::ShaderDataType::Mat3:	return GL_FLOAT;
        case Polarity::ShaderDataType::Mat4:	return GL_FLOAT;
        case Polarity::ShaderDataType::Int:		return GL_INT;
        case Polarity::ShaderDataType::Int2:	return GL_INT;
        case Polarity::ShaderDataType::Int3:	return GL_INT;
        case Polarity::ShaderDataType::Int4:	return GL_INT;
        case Polarity::ShaderDataType::Bool:	return GL_BOOL;
        }

        LOG_MAJOR_ERROR("Unknown ShaderDataType !!!");
        return 0;
    }

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

        float vert[3 * 7] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 1.0f, 1.0f,
            0.5f,  -0.5f, 0.0f,     0.0f, 1.0f, 1.0f, 1.0f,
            0.0f,  0.5f,  0.0f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        m_vertexBuffer.reset(VertexBuffer::Create(vert, sizeof(vert)));

        {
            BufferLayout layout = {
               { ShaderDataType::Float3, "a_Position" },
               { ShaderDataType::Float4, "a_Color"    }
            };

            m_vertexBuffer->SetLayout(layout);
        }

        uint32_t index = 0;
        const auto& layout = m_vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,                        
                element.GetComponentCount(), 
                ShaderDataTypeToOpenGLBaseType(element.Type),   
                element.Normalized ? GL_TRUE : GL_FALSE, 
                layout.GetStride(),
                (const void*)element.Offset);
            
            index++;
        }

     
        uint32_t indices[3] = { 0,1,2 };
        m_indexBuffer.reset(IndexBuffer::Create(indices, 3));


        std::string vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
	        gl_Position = vec4(a_Position, 1.0);
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
            glClearColor(0.1f, 0.1f, 0.1f, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_shader->Bind();
            glBindVertexArray(m_vertexArray);
            glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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
