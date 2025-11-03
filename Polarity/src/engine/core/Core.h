#pragma once

#ifdef POLARITY_PLATFORM_WINDOWS
	#define DEBUG_BREAK() __debugbreak()
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

#if POLARITY_DYNAMIC_LINK
	#ifdef POLARITY_BUILD_DLL
		#define POLARITY_API __declspec(dllexport)
	#else
		#define POLARITY_API __declspec(dllimport)
	#endif
#else
	#define POLARITY_API
#endif

#elif POLARITY_PLATFORM_LINUX
	#error We only support Windows not Linux!!!

#elif POLARITY_PLATFORM_MAC
	#error We only support Windows not Mac!!!
#else
	#error Unknown platform!!!
#endif


#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

#define POLARITY_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)