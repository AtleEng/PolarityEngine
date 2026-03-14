#include "polpch.h"
#include "Application.h"

#include "Input.h"
#include "engine/renderer/Renderer.h"
#include "engine/audio/Audio.h"
#include "engine/utils/Random.h"
#include "engine//utils/PlatformUtils.h"

#include <filesystem>

namespace Polarity {


	Application* Application::s_instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		:m_Specification(specification)
	{
		POL_PROFILE_FUNCTION();

		Logger::Init();
		POL_CORE_ASSERT(!s_instance, "Core: Application already exist!");
		s_instance = this;

		std::string info = "Core: PolarityEngine v0.01 ";

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
		POL_CORE_INFO(info.c_str());


		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_window = Window::Create(WindowProps(m_Specification.Name));
		m_window->SetEventCallback(POLARITY_BIND_EVENT_FN(Application::OnEvent));

		
		Renderer::Init();
		Audio::Init();
		Random::Init();

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
			if (e.handled)
			{
				break;
			}
			(*--i)->OnEvent(e);
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
			POLARITY_PROFILE_SCOPE("Full Cycle");

			float time = Time::GetTime();
			Timestep timeStep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized)
			{
				for (Layer* layer : m_layerStack)
				{
					POLARITY_PROFILE_SCOPE("OnUpdate");
					layer->OnUpdate(timeStep);
				}

				m_imGuiLayer->Begin();
				for (Layer* layer : m_layerStack)
				{
					POLARITY_PROFILE_SCOPE("OnImGUIRender");
					layer->OnImGuiRender();
				}
				m_imGuiLayer->End();
			}
			m_window->OnUpdate();
		}
	}

	void Application::Shutdown()
	{
		POL_CORE_INFO("Core: Shutdown...");
		m_running = false;
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
		return false;
	}
}
