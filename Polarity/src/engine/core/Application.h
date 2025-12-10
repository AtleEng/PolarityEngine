#pragma once
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timestep.h"

#include "engine/events/Event.h"
#include "engine/events/ApplicationEvent.h"

#include "engine/imGui/ImGuiLayer.h"

namespace Polarity {


	class Application
	{
	public:
		Application(const WindowProps props = WindowProps("App"));
		virtual ~Application();

		void Run();
		void Shutdown();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_window; }

		static Application& Get() { return *s_instance; }
		ImGuiLayer& GetImGuiLayer() { return *m_imGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:           
		std::unique_ptr<Window> m_window;

		bool m_running = true;
		bool m_minimized = false;

		LayerStack m_layerStack;
		ImGuiLayer* m_imGuiLayer;

		float m_lastFrameTime = 0.0f;

	private:
		static Application* s_instance;
	};

	//defined in client
	Application* CreateApplication();
}


