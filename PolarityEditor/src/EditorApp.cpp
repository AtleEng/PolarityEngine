#include <Polarity.h>
#include <engine/core/EntryPoint.h>

#include "EditorLayer.h"

namespace Polarity
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& spec)
			: Application(spec) //WindowProps("Polarity Editor", 1280, 720, true, true)
		{
			POL_PROFILE_FUNCTION();

			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Polarity Editor";
		spec.CommandLineArgs = args;

		return new EditorApp(spec);
	}
}