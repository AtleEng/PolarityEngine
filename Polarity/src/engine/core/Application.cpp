#include "polpch.h"
#include "Application.h"

#include "engine/renderer/Renderer.h"
#include "Input.h"

#include <GLFW/glfw3.h>  //temp

namespace Polarity {


	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		LOG_ASSERT(!s_instance, "Application already exist !!!");
		s_instance = this;


		std::string info = "v0.01 ";

		info += "- ";
		info += POLARITY_CONFIG;
		info += " - ";
		info += POLARITY_PLATFORM;
		info += " ";
		info += POLARITY_ARCH;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:		info += " - None"; break;
		case RendererAPI::API::OpenGL:		info += " - OpenGL"; break;
		default:							info += " - Unknown"; break;
		}
		info += "\n";
		LOG_EX("PolarityEngine", TextColor::Orange, info.c_str());


		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(POLARITY_BIND_EVENT_FN(OnEvent));


		Renderer::Init();

		// ImGui
		m_imGuiLayer = new ImGuiLayer();
		PushOverlay(m_imGuiLayer);
	}

	Application::~Application() = default;


	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(POLARITY_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(POLARITY_BIND_EVENT_FN(Application::OnWindowResize));

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
			float time = (float)glfwGetTime();  //TODO temporary (abstract function to plattform.h)
			Timestep timeStep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized)
			{
				for (Layer* layer : m_layerStack)
				{
					layer->OnUpdate(timeStep);
				}

				m_imGuiLayer->Begin();
				for (Layer* layer : m_layerStack)
				{
					layer->OnImGuiRender();
				}
				m_imGuiLayer->End();
			}
			m_window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;

		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		LOG_DEBUG("Window resized: (%f, %f)", e.GetWidth(), e.GetHeight())
			return false;
	}
}
