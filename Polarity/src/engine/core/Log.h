#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "Core.h"

// ################################     Logging    ################################
#pragma region
enum class TextColor
{
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Orange,
    Pink,
    Aqua,
    Count
};

extern const char* TextColorTable[(int)TextColor::Count];



void _log(const char* prefix,TextColor textColor, const char* msg, ...);
std::string GetTimeString();

#define LOG(msg, ...)                       _log("[LOG]   ", TextColor::White, msg, ##__VA_ARGS__);
#define LOG_EX(prefix, textColor, msg, ...) _log(prefix, textColor, msg, ##__VA_ARGS__);

#define LOG_TRACE(msg, ...)                 _log("[TRACE] ", TextColor::Green, msg, ##__VA_ARGS__);
#define LOG_DEBUG(msg, ...)                 _log("[DEBUG] ", TextColor::Yellow, msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...)                  _log("[INFO]  ", TextColor::Cyan, msg, ##__VA_ARGS__);
                                                             
#define LOG_WARN(msg, ...)                  _log("[WARN]  ", TextColor::Orange, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...)                 _log("[ERROR] ", TextColor::Red, msg, ##__VA_ARGS__);
#define LOG_MAJOR_ERROR(msg, ...)           LOG_ERROR(msg, ##__VA_ARGS__);  DEBUG_BREAK();

#ifdef POLARITY_ENABLE_ASSERTS
#define LOG_ASSERT(x, msg, ...)             \
	{										\
		if (!(x))                           \
		{                                   \
		LOG_ERROR(msg, ##__VA_ARGS__);		\
		DEBUG_BREAK();						\
		}                                   \
	}
#else
#define LOG_ASSERT(x, msg, ...)
#endif

