

/*Script - bootloader.c*/
// by:-  Ashutosh tiwari

#include "bootloader.h"
#include "state_manager.h"
#include "memory_manager.h"
#include "partition_manager.h"
#include "flash_manager.h"
#include "crc.h"
#include "reboot_manager.h"
#include "log_manager.h"
#include <stdio.h>
#include <string.h>

static bootloader_error_t last_error = BOOTLOADER_NO_ERROR;

static void SetError(bootloader_error_t error) {
    last_error = error;
    current_state = BOOTLOADER_ERROR;
    LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Error encountered: %d", error);
}

void Bootloader_Init(void) {
    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Starting initialization...");

    RebootManager_Init();
    MemoryManager_Init();
    StateManager_Init();
    Partition_Init();

    if (Partition_Validate(PARTITION_ACTIVE)) {
        current_state = BOOTLOADER_BOOT;
        LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Active partition is valid. Ready to boot.");
    } else if (Partition_Validate(PARTITION_INACTIVE)) {
        current_state = BOOTLOADER_ROLLBACK;
        LogManager_Log(LOG_LEVEL_WARNING, "[Bootloader] Active partition invalid, initiating rollback.");
    } else {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] No valid firmware found in either partition.");
        SetError(BOOTLOADER_ERR_NO_VALID_FW);
        current_state = BOOTLOADER_IDLE;  // Enter Idle State if rollback fails
    }
}

void CheckUpdateState(void) {
    if (StateManager_IsInState(BOOTLOADER_UPDATE)) {
        LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Firmware update in progress...");
    } else {
        LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Not in update state.");
    }
}

bool Bootloader_FirmwareUpdate(const uint8_t* firmware, uint32_t size) {
    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Starting firmware update...");

    if (size > PARTITION_SIZE) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Firmware size exceeds partition size.");
        SetError(BOOTLOADER_ERR_FW_UPDATE_FAIL);
        return false;
    }

    if (Flash_ProgramPage(PARTITION_INACTIVE_START, firmware, size) != FLASH_SUCCESS) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Flash programming failed.");
        SetError(BOOTLOADER_ERR_FW_UPDATE_FAIL);
        return false;
    }

    Partition_Validate(PARTITION_INACTIVE);
    current_state = BOOTLOADER_UPDATE;
    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Firmware update completed successfully.");
    return true;
}

bool Bootloader_ValidateAndBoot(void) {
    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Validating active firmware...");

    if (!Partition_Validate(PARTITION_ACTIVE)) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Active partition is invalid.");
        SetError(BOOTLOADER_ERR_FW_UPDATE_FAIL);
        current_state = BOOTLOADER_ROLLBACK;
        RebootManager_PrepareReboot("Active partition invalid");
        return false;
    }

#ifdef USE_CRC_VALIDATION
    if (!CRC_Validate(PARTITION_ACTIVE_START, PARTITION_SIZE)) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] CRC validation failed for active firmware.");
        SetError(BOOTLOADER_ERR_CRC_MISMATCH);
        RebootManager_PrepareReboot("CRC validation failed");
        return false;
    }
#endif

    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Booting active firmware...");
    current_state = BOOTLOADER_BOOT;

    MemoryManager_Deinit();
    return true;
}

void Bootloader_Rollback(void) {
    LogManager_Log(LOG_LEVEL_WARNING, "[Bootloader] Initiating rollback...");

    if (!Partition_SwitchActive()) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Rollback failed: Unable to switch partition.");
        SetError(BOOTLOADER_ERR_PARTITION_SWITCH_FAIL);
        current_state = BOOTLOADER_IDLE;  // Enter Idle State
        return;
    }

    if (!Partition_Validate(PARTITION_ACTIVE)) {
        LogManager_Log(LOG_LEVEL_ERROR, "[Bootloader] Rollback failed: No valid firmware in either partition.");
        SetError(BOOTLOADER_ERR_NO_VALID_FW);
        current_state = BOOTLOADER_IDLE;  // Enter Idle State
        return;
    }

    current_state = BOOTLOADER_BOOT;
    LogManager_Log(LOG_LEVEL_INFO, "[Bootloader] Rollback successful. Ready to boot.");
}


bootloader_state_t Bootloader_GetState(void) {
    return current_state;
}

bootloader_error_t Bootloader_GetLastError(void) {
    return last_error;
}


