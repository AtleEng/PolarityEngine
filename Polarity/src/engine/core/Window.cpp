#include "polpch.h"
#include "Window.h"

#ifdef POLARITY_PLATFORM_WINDOWS
#include "platform/windows/WindowsWindow.h"
#endif


namespace Polarity
{
	Window* Window::Create(const WindowProps& props)
	{
		#ifdef POLARITY_PLATFORM_WINDOWS
			return new WindowsWindow(props);
		#else
			LOG_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}