#include "atlaspch.h"
#include "Application.h"
#include "Window.h"

#include "engine/Log.h"

#include <GLFW/glfw3.h>

namespace Atlas {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    // Private implementation struct
    struct Application::Impl
    {
        std::unique_ptr<Window> m_window;

        Impl()
        {
            m_window = std::unique_ptr<Window>(Window::Create());
        }
    };

    Application::Application()
        : m_impl(std::make_unique<Impl>())
    {
        m_impl->m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
    }

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
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_layerStack.PushOverlay(layer);
    }

    void Application::Run()
    {
        while (m_running)
        {
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_layerStack)
            {
                layer->OnUpdate();
            }
            m_impl->m_window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        LOG_INFO("Shutdown....");
        m_running = false;

        return true;
    }
}
