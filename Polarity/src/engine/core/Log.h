#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "Core.h"

// ################################     Logging    ################################

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


void _log(const char* prefix,TextColor textColor, bool shouldNotify, const char* msg, ...);
std::string GetTimeString();



struct LogEvent
{
    TextColor color;
    std::string prefix;
    std::string message;
    std::string time;
};

using LogListener = void(*)(const LogEvent&);

void AddLogListener     (LogListener listener);
void RemoveLogListener  (LogListener listener);



#define LOG(msg, ...)                       _log("LOG   ", TextColor::White,  true, msg, ##__VA_ARGS__);
#define LOG_EX(prefix, textColor, msg, ...) _log(prefix,     textColor,         true, msg, ##__VA_ARGS__);

#define LOG_TRACE(msg, ...)                 _log("TRACE ", TextColor::Green,  true, msg, ##__VA_ARGS__);
#define LOG_DEBUG(msg, ...)                 _log("DEBUG ", TextColor::Yellow, true, msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...)                  _log("INFO  ", TextColor::Cyan,   true, msg, ##__VA_ARGS__);
                                                             
#define LOG_WARN(msg, ...)                  _log("WARN  ", TextColor::Orange, true, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...)                 _log("ERROR ", TextColor::Red,    true, msg, ##__VA_ARGS__);
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

