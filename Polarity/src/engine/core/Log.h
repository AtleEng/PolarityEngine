#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "Core.h"

namespace Polarity
{
	struct LogEvent
	{
		int color;
		std::string prefix;
		std::string message;
		std::string time;
	};

	class Logger {
		using LogListener = std::function<void(const LogEvent&)>;
	public:
		static std::vector<LogListener> g_Listeners;

		enum class TextColor
		{
			White,
			Cyan,
			Green,
			Yellow,
			Orange,
			Red,
			Count
		};
		static const char* TextColorTable[(int)TextColor::Count];

		static void Init();

		static void Log(const char* prefix, TextColor textColor, const char* msg, ...);
		
		static void AddLogListener(LogListener listener);
		static void RemoveLogListener(LogListener listener);

	private:
		static std::string GetTimeString();
	};
}

#define POL_CORE_TRACE(msg, ...)                 Polarity::Logger::Log("TRACE ", Polarity::Logger::TextColor::Green,  msg, ##__VA_ARGS__);
#define POL_CORE_DEBUG(msg, ...)                 Polarity::Logger::Log("DEBUG ", Polarity::Logger::TextColor::Yellow, msg, ##__VA_ARGS__);
#define POL_CORE_INFO(msg, ...)                  Polarity::Logger::Log("INFO  ", Polarity::Logger::TextColor::Cyan,   msg, ##__VA_ARGS__);                                                      
#define POL_CORE_WARN(msg, ...)                  Polarity::Logger::Log("WARN  ", Polarity::Logger::TextColor::Orange, msg, ##__VA_ARGS__);
#define POL_CORE_ERROR(msg, ...)                 Polarity::Logger::Log("ERROR ", Polarity::Logger::TextColor::Red,    msg, ##__VA_ARGS__);
#define POL_CORE_FATAL(msg, ...)                 POL_CORE_ERROR(msg, ##__VA_ARGS__);  DEBUG_BREAK();

#ifdef POLARITY_ENABLE_ASSERTS
#define POL_CORE_ASSERT(x, msg, ...)            \
	{										    \
		if (!(x))                               \
		{                                       \
		POL_CORE_FATAL(msg, ##__VA_ARGS__);		\
		}                                       \
	}
#else
#define POL_CORE_ASSERT(x, msg, ...)
#endif

