#include "polpch.h"
#include "Log.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <functional>

namespace Polarity {

	std::vector<Logger::LogListener> Logger::g_Listeners;

	const char* Logger::TextColorTable[(int)TextColor::Count] =
	{
		"\x1b[37m",       // WHITE
		"\x1b[36m",       // CYAN
		"\x1b[32m",       // GREEN
		"\x1b[33m",       // YELLOW
		"\x1b[38;5;208m", // ORANGE
		"\x1b[31m",       // RED
	};

	std::string Logger::GetTimeString()
	{
		using namespace std::chrono;

		auto now = system_clock::now();
		auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		std::time_t t = system_clock::to_time_t(now);

		std::tm localTime;
#if defined(POLARITY_PLATFORM_WINDOWS)
		localtime_s(&localTime, &t);
#else
		localtime_r(&t, &localTime);
#endif

		std::stringstream ss;
		ss << std::put_time(&localTime, "%H:%M:%S")
			<< '.' << std::setw(3) << std::setfill('0') << ms.count();

		return ss.str();
	}

	void Logger::AddLogListener(LogListener listener)
	{
		g_Listeners.push_back(listener);
	}

	void Logger::RemoveLogListener(LogListener listener) //TODO fix
	{
		//g_Listeners.erase(std::remove(g_Listeners.begin(), g_Listeners.end(), listener));
	}

	void StdoutLogListener(const LogEvent& event)
	{
		printf("%s%s %s%s%s %s\033[0m\n",
			Logger::TextColorTable[(int)Logger::TextColor::White],
			event.time.c_str(),
			Logger::TextColorTable[event.color],
			event.prefix.c_str(),
			Logger::TextColorTable[(int)Logger::TextColor::White],
			event.message.c_str()
		);
	}

	void Logger::Init()
	{
		AddLogListener(StdoutLogListener);
	}

	void Logger::Log(const char* prefix, TextColor textColor, const char* msg, ...)
	{
		POLARITY_PROFILE_FUNCTION();

		// ---- format message ----
		va_list args;
		va_start(args, msg);

		va_list copy;
		va_copy(copy, args);
		int len = vsnprintf(nullptr, 0, msg, copy);
		va_end(copy);

		std::string message(len, '\0');
		vsnprintf(message.data(), message.size() + 1, msg, args);
		va_end(args);

		LogEvent event;
		event.color = (int)textColor;
		event.prefix = prefix;
		event.message = std::move(message);
		event.time = GetTimeString();

		// ---- dispatch ----
		for (auto& listener : g_Listeners)
			listener(event);
	}
}