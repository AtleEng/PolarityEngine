#pragma once

#include <memory>

#ifdef POLARITY_PLATFORM_WINDOWS
	#define DEBUG_BREAK() __debugbreak()
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

#elif POLARITY_PLATFORM_LINUX
	#define DEBUG_BREAK() __builtin_debugtrap()
	#error We only support Windows not Linux !!!

#elif POLARITY_PLATFORM_MAC
	#define DEBUG_BREAK() __builtin_trap()
	#error We only support Windows not Mac !!!
#else
	#error Unknown platform!!!
#endif


#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

#define POLARITY_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Polarity
{
	// Works like a unique_ptr but for Polarity resources
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// Works like a shared_ptr but for Polarity resources
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}