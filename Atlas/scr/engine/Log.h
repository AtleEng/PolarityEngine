#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "core.h"

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

enum class LogPrefix
{
    Trace,
    Debug,
    Info,
    Warn,
    Error
};

ATLAS_API void Log(LogPrefix prefix, const char* msg, ...);