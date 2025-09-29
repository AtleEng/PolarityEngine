#include "Application.h"

#include "engine/Events/ApplicationEvent.h"
#include "engine/Log.h"

namespace Atlas {

	Application::Application() {
		// constructor code
	}

	Application::~Application() {
		// destructor code (can be empty if nothing special is needed)
	}

	void Application::Run() 
	{
		WindowResizeEvent e(1280, 720);
		LOG_DEBUG(e.ToString().c_str());

		while (true);
	}
}
