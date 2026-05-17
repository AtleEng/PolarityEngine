#pragma once
#include "Core.h"

#include "Window.h"
#include "LayerStack.h"
#include "Timestep.h"
#include "Log.h"

#include "engine/events/Event.h"
#include "engine/events/ApplicationEvent.h"

#include "engine/imGui/ImGuiLayer.h"
#include "engine/scripting/ScriptingInput.h"

namespace Polarity {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			POL_CORE_ASSERT(index < Count, "?");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Polarity App";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();
		void Shutdown();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_window; }
		ScriptingInput& GetInput() { return *m_Input; }

		static Application& Get() { return *s_instance; }
		ImGuiLayer& GetImGuiLayer() { return *m_imGuiLayer; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:    
		ApplicationSpecification m_Specification;
		Scope<Window> m_window;
		Scope<ScriptingInput> m_Input;

		bool m_running = true;
		bool m_minimized = false;

		LayerStack m_layerStack;
		ImGuiLayer* m_imGuiLayer;

		float m_lastFrameTime = 0.0f;

	private:
		static Application* s_instance;
	};

	//defined in client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}


