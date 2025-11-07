#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Core.h"

// ################################     Logging    ################################
#pragma region
enum TextColor
{
    textColorBlack,
    textColorRed,
    textColorGreen,
    textColorYellow,
    textColorBlue,
    textColorMagenta,
    textColorCyan,
    textColorWhite,
    textColorOrange,
    textColorPink,
    textColorAqua,
    textColorCount
};

extern const char* TextColorTable[textColorCount];



POLARITY_API void _log(const char* prefix,TextColor textColor, const char* msg, ...);

#define LOG(msg, ...)                       _log("[LOG]   ", textColorWhite, msg, ##__VA_ARGS__);
#define LOG_EX(prefix, textColor, msg, ...) _log(prefix, textColor, msg, ##__VA_ARGS__);

#define LOG_TRACE(msg, ...)                 _log("[TRACE] ", textColorGreen, msg, ##__VA_ARGS__);
#define LOG_DEBUG(msg, ...)                 _log("[DEBUG] ", textColorYellow, msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...)                  _log("[INFO]  ", textColorAqua, msg, ##__VA_ARGS__);

#define LOG_WARN(msg, ...)                  _log("[WARN]  ", textColorOrange, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...)                 _log("[ERROR] ", textColorRed, msg, ##__VA_ARGS__);
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

