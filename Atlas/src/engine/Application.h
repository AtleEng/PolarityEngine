#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include <memory>

namespace Atlas {


	class ATLAS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		struct Impl;                  
		std::unique_ptr<Impl> m_impl; // safe
		bool m_running = true;
	};

	//defined in client
	Application* CreateApplication();
}


