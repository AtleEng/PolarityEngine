#pragma once
#include "Core.h"
#include "Window.h"
#include "engine/LayerStack.h"
#include "events/Event.h"
#include "events/ApplicationEvent.h"

#include <memory>

namespace Atlas {


	class ATLAS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
                 
		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layerStack;

		static Application* s_instance;
	};

	//defined in client
	Application* CreateApplication();
}


