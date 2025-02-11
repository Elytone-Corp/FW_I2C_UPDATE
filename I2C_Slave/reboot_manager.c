



/*Script - reboot_manager.c*/
// by:-  Ashutosh tiwari



#include "reboot_manager.h"
#include "log_manager.h"
#include "memory_manager.h"
#include "max32660.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char last_reboot_reason[64] = "UNKNOWN";

void RebootManager_Init(void) {
    LogManager_Log(LOG_LEVEL_INFO, "[RebootManager] Initialization complete.");
}

void RebootManager_PrepareReboot(const char* reason) {
    LogManager_Log(LOG_LEVEL_INFO, "[Reboot Manager] Preparing system for reboot...");

    strncpy(last_reboot_reason, reason, sizeof(last_reboot_reason) - 1);
    last_reboot_reason[sizeof(last_reboot_reason) - 1] = '\0';

    if (!Memory_Cleanup()) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Reboot Manager] Memory Cleanup failed.");
    }

    LogManager_Log(LOG_LEVEL_INFO, "[Reboot Manager] System ready for reboot.");
}

void RebootManager_Reboot(void) {
    LogManager_Log(LOG_LEVEL_INFO, "[RebootManager] Triggering system reboot...");
    LogManager_Log(LOG_LEVEL_DEBUG, "[RebootManager] Last reboot reason: %s", last_reboot_reason);

    NVIC_SystemReset();
}

