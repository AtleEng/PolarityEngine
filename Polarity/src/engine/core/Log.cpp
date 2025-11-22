#include "polpch.h"
#include "Log.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

const char* TextColorTable[TextColor::Count] =
{
    "\x1b[30m",       // BLACK
    "\x1b[31m",       // RED
    "\x1b[32m",       // GREEN
    "\x1b[33m",       // YELLOW
    "\x1b[34m",       // BLUE
    "\x1b[35m",       // MAGENTA
    "\x1b[36m",       // CYAN
    "\x1b[37m",       // WHITE
    "\x1b[38;5;208m", // ORANGE
    "\x1b[38;5;201m", // PINK
    "\x1b[38;5;51m",  // AQUA
};


void _log(const char* prefix, TextColor textColor, const char* msg, ...)
{
    char textBuffer[8192] = {};

    va_list args;
    va_start(args, msg);

    // Combine prefix and color with the user format string
    snprintf(textBuffer, sizeof(textBuffer), "%s%s %s%s %s", TextColorTable[(int)TextColor::White], GetTimeString().c_str(), TextColorTable[(int)textColor], prefix, TextColorTable[(int)TextColor::White]);

    // Append the user-provided formatted string
    size_t len = strlen(textBuffer);
    vsnprintf(textBuffer + len, sizeof(textBuffer) - len, msg, args);

    va_end(args);

    // Reset color
    strcat_s(textBuffer, "\033[0m");

    puts(textBuffer);
}

std::string GetTimeString()
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

