#pragma once

#ifdef ATLAS_PLATFORM_WINDOWS
	#define DEBUG_BREAK() __debugbreak()
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

#if ATLAS_DYNAMIC_LINK
	#ifdef ATLAS_BUILD_DLL
		#define ATLAS_API __declspec(dllexport)
	#else
		#define ATLAS_API __declspec(dllimport)
	#endif
#else
	#define ATLAS_API
#endif

#elif ATLAS_PLATFORM_LINUX
	#error We only support Windows not Linux!!!

#elif ATLAS_PLATFORM_MAC
	#error We only support Windows not Mac!!!
#else
	#error Unknown platform!!!
#endif


#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

#define ATLAS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)