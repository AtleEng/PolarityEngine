#pragma once
#include "Core.h"
#include <memory>

namespace Atlas {

	class Window; //forward declaration

	class ATLAS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		struct Impl;                  // opaque implementation
		std::unique_ptr<Impl> m_impl; // safe
		bool m_running = true;
	};

	//defined in client
	Application* CreateApplication();
}


