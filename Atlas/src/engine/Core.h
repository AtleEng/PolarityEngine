#pragma once

#ifdef ATLAS_PLATFORM_WINDOWS
	#define DEBUG_BREAK() __debugbreak()
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

	#ifdef ATLAS_BUILD_DLL
		#define ATLAS_API __declspec(dllexport)
	#else
		#define ATLAS_API __declspec(dllimport)
	#endif
#else
	#error We only support Windows!!!
#endif // ATLAS_PLATFORM_WINDOWS

#ifdef ATLAS_ENABLE_ASSERTS
	#define LOG_ASSERT(x, msg, ...)         \
	{										\
		if (!(x))                           \
		{                                   \
		LOG_ERROR(msg, ##__VA_ARGS__);		\
		DEBUG_BREAK();						\
		}                                   \
	}
#else
#define LOG_ASSERT(x, msg, ...)

#endif // ATLAS_ENABLE_ASSERTS


#define BIT(x) (1 << x)

/*
Github add

git add *
git status
git commit -m "text"
git push origin master

*/