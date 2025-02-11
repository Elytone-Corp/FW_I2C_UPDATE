


/*Script - log_manager.c*/
// by:-  Ashutosh tiwari



#include "log_manager.h"
#include <stdarg.h>
#include <time.h>

static log_level_t current_log_level = LOG_LEVEL_INFO;

void LogManager_Init(void) {
    // Initialize logging system if needed
    current_log_level = LOG_LEVEL_INFO; // Default level
}

void LogManager_Log(log_level_t level, const char* format, ...) {
    if (level < current_log_level) {
        return; // Skip logs below the current log level
    }

    const char* level_str = "";
    switch (level) {
        case LOG_LEVEL_INFO:
            level_str = "[INFO]";
            break;
        case LOG_LEVEL_DEBUG:
            level_str = "[DEBUG]";
            break;
        case LOG_LEVEL_WARNING:
            level_str = "[WARNING]";
            break;
        case LOG_LEVEL_ERROR:
            level_str = "[ERROR]";
            break;
    }

    time_t now;
    time(&now);
    struct tm* local_time = localtime(&now);
    printf("%02d:%02d:%02d %s ", local_time->tm_hour, local_time->tm_min, local_time->tm_sec, level_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
