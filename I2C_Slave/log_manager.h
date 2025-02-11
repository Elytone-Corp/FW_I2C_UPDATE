


/*Script - log_manager.h*/
// by:-  Ashutosh tiwari


#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Ensure Logger_Info maps to LogManager_Log
#define Logger_Info(format, ...) LogManager_Log(LOG_LEVEL_INFO, format, ##__VA_ARGS__)

/**
 * Log levels definition.
 */
typedef enum {
    LOG_LEVEL_INFO = 0,     // Informational messages
    LOG_LEVEL_DEBUG,        // Debugging messages
    LOG_LEVEL_WARNING,      // Warning messages
    LOG_LEVEL_ERROR         // Error messages
} log_level_t;

void LogManager_Init(void);

void LogManager_Log(log_level_t level, const char* format, ...);

void LogManager_SetLevel(log_level_t level);

#endif // LOG_MANAGER_H

