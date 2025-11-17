#pragma once
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timestep.h"

#include "../events/Event.h"
#include "../events/ApplicationEvent.h"

#include "../imGui/ImGuiLayer.h"
#include "engine/renderer/Shader.h"

#include "engine/renderer/Shader.h"
#include "engine/renderer/Buffer.h"
#include "engine/renderer/VertexArray.h"

namespace Polarity {


	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_window; }

		inline static Application& Get() { return *s_instance; }
		inline ImGuiLayer& GetImGuiLayer() { return *m_imGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:           
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		float m_lastFrameTime = 0.0f;

	private:
		static Application* s_instance;
	};

	//defined in client
	Application* CreateApplication();
}


