#include <Polarity.h>
#include <engine/core/EntryPoint.h>
#include "imgui/imgui.h"
#include "DemoLayer.h"

namespace Polarity
{
	class Sandbox : public Application
	{
	public:
		Sandbox()
			: Application(WindowProps("Sandbox", 1280, 720, false, false))
		{
			POLARITY_PROFILE_FUNCTION();

			PushLayer(new DemoLayer());

			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
		}
		~Sandbox()
		{

		}

	};

	Application* CreateApplication()
	{
		return new Sandbox();
	}
}