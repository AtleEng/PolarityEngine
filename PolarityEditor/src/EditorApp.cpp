#include <Polarity.h>
#include <engine/core/EntryPoint.h>

#include "EditorLayer.h"

namespace Polarity
{
	class EditorApp : public Application
	{
	public:
		EditorApp()
			: Application(WindowProps("Polarity Editor", 1280, 720, true, true))
		{
			POLARITY_PROFILE_FUNCTION();

			PushLayer(new EditorLayer());
		}
		~EditorApp()
		{

		}
	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}
}