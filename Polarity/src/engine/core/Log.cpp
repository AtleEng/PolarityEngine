#include "polpch.h"
#include "Log.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

static std::vector<LogListener> g_Listeners;

const char* TextColorTable[(int)TextColor::Count] =
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


void _log(const char* prefix, TextColor textColor, bool shouldNotify, const char* msg, ...)
{
    POLARITY_PROFILE_FUNCTION();

    char messageBuffer[4096];

    va_list args;
    va_start(args, msg);
    vsnprintf(messageBuffer, sizeof(messageBuffer), msg, args);
    va_end(args);

    LogEvent event;
    event.color = textColor;
    event.prefix = prefix;
    event.message = messageBuffer;
    event.time = GetTimeString();

    // Console formatting
    printf("%s%s %s%s%s %s\033[0m\n",
        TextColorTable[(int)TextColor::White],
        event.time.c_str(),
        TextColorTable[(int)textColor],
        event.prefix.c_str(),
        TextColorTable[(int)TextColor::White],
        event.message.c_str()
    );

    if (shouldNotify)
    {
        for (auto& listener : g_Listeners)
            listener(event);
    }
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

void AddLogListener(LogListener listener)
{
    g_Listeners.push_back(listener);
}

void RemoveLogListener(LogListener listener)
{
    g_Listeners.erase(std::remove(g_Listeners.begin(), g_Listeners.end(), listener));
}

