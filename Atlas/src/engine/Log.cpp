#include "atlaspch.h"
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


void _log(const char* prefix, TextColor textColor, const char* msg, ...)
{
    char textBuffer[8192] = {};

    va_list args;
    va_start(args, msg); // last named parameter before: ...

    // Combine prefix and color with the user format string
    snprintf(textBuffer, sizeof(textBuffer), "%s %s ", TextColorTable[textColor], prefix);

    // Append the user-provided formatted string
    size_t len = strlen(textBuffer);
    vsnprintf(textBuffer + len, sizeof(textBuffer) - len, msg, args);

    va_end(args);

    // Reset color
    strcat_s(textBuffer, "\033[0m");

    puts(textBuffer);
}

