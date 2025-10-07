#include "atlaspch.h"
#include "engine/Window.h"

#ifdef ATLAS_PLATFORM_WINDOWS
#include "platform/windows/WindowsWindow.h"
#endif


namespace Atlas
{
	Window* Window::Create(const WindowProps& props)
	{
		#ifdef ATLAS_PLATFORM_WINDOWS
			return new WindowsWindow(props);
		#else
			LOG_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}