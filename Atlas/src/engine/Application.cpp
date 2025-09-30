#include "atlaspch.h"
#include "Application.h"
#include "Window.h"

#include "engine/Events/Event.h"
#include "engine/Log.h"

namespace Atlas {

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
    }

    Application::~Application() = default;

    void Application::Run()
    {
        while (true) // simple run loop
        {
            m_impl->m_window->OnUpdate();
            // handle events here...
        }
    }
}
