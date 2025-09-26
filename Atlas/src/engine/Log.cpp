#include "Log.h"

const char* TextColorTable[textColorCount] =
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

static const char* PrefixText[] =
{
    "TRACE:",
    "DEBUG:",
    "INFO:",
    "WARN:",
    "ERROR:"
};
static TextColor PrefixColor[] =
{
    textColorGreen,   // Trace
    textColorPink,    // Debug
    textColorAqua,    // Info
    textColorOrange,  // Warn
    textColorRed      // Error
};

void Log(LogPrefix prefix, const char* msg, ...)
{
    char buffer[8192] = {};

    // Prefix with color + label
    int written = snprintf(buffer, sizeof(buffer), "%s %s ",
        TextColorTable[PrefixColor[(int)prefix]],
        PrefixText[(int)prefix]);

    if (written < 0 || written >= (int)sizeof(buffer))
        return; // failed or truncated badly

    // Append formatted message
    va_list args;
    va_start(args, msg);
    int appended = vsnprintf(buffer + written,
        sizeof(buffer) - written,
        msg, args);
    va_end(args);

    if (appended < 0 || written + appended >= (int)sizeof(buffer))
        return; // failed or truncated badly

    // Append reset color
    snprintf(buffer + written + appended,
        sizeof(buffer) - (written + appended),
        "\033[0m");

    puts(buffer);
}