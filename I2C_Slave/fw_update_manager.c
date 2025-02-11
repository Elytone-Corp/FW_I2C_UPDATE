

/*Script -- fw_update_manager.c*/
// Edited by :-  Ashutosh Tiwari



#include "fw_update_manager.h"
#include "flash_manager.h"
#include "crc.h"
#include "partition_manager.h"
#include <stdio.h>
#include <string.h>

// Static variables to track the current firmware update state
static uint32_t current_write_address = PARTITION_INACTIVE_START;
static uint32_t total_written_bytes = 0;

void FWUpdateManager_Init(void) {
    printf("[FW Update Manager] Initialization complete.\n");
    current_write_address = PARTITION_INACTIVE_START;
    total_written_bytes = 0;
}

cmd_response_t FWUpdateManager_StartUpdate(void) {
    printf("[FW Update Manager] Starting firmware update...\n");

    if (Flash_ErasePage(PARTITION_INACTIVE_START) != FLASH_SUCCESS) {
        printf("[FW Update Manager] Error: Failed to erase inactive partition.\n");
        return CMD_ERR_FLASH_OP_FAILED;
    }

    current_write_address = PARTITION_INACTIVE_START;
    total_written_bytes = 0;

    return CMD_SUCCESS;
}

cmd_response_t FWUpdateManager_ProcessChunk(const uint8_t* data, uint32_t length) {
    if (data == NULL || length == 0) {
        printf("[FW Update Manager] Error: Invalid data chunk.\n");
        return CMD_ERR_INVALID_COMMAND;
    }

    printf("[FW Update Manager] Writing chunk at address 0x%08X...\n", current_write_address);

    if (current_write_address + length > PARTITION_INACTIVE_START + PARTITION_SIZE) {
        printf("[FW Update Manager] Error: Write exceeds partition size.\n");
        return CMD_ERR_FLASH_OP_FAILED;
    }

    if (Flash_ProgramPage(current_write_address, data, length) != FLASH_SUCCESS) {
        printf("[FW Update Manager] Error: Flash programming failed.\n");
        return CMD_ERR_FLASH_OP_FAILED;
    }

    current_write_address += length;
    total_written_bytes += length;

    return CMD_SUCCESS;
}

cmd_response_t FWUpdateManager_FinalizeUpdate(void) {
    printf("[FW Update Manager] Finalizing firmware update...\n");

    if (!CRC_UpdateExpected(PARTITION_INACTIVE_START, total_written_bytes)) {
        printf("[FW Update Manager] Error: CRC calculation/storage failed.\n");
        return CMD_ERR_CRC_MISMATCH;
    }

    Partition_Validate(PARTITION_INACTIVE);

    if (!Partition_SwitchActive()) {
        printf("[FW Update Manager] Error: Failed to activate partition.\n");
        return CMD_ERR_PARTITION_SWITCH_FAIL;
    }

    printf("[FW Update Manager] Firmware update completed and validated.\n");
    return CMD_SUCCESS;
}

void FWUpdateManager_CancelUpdate(void) {
    printf("[FW Update Manager] Canceling firmware update...\n");

    current_write_address = PARTITION_INACTIVE_START;
    total_written_bytes = 0;

    Partition_MarkRollback(PARTITION_INACTIVE);
}

bool FWUpdateManager_VerifyIntegrity(void) {
    printf("[FW Update Manager] Verifying integrity of the inactive partition...\n");

    if (!Partition_IsValid(PARTITION_INACTIVE)) {
        printf("[FW Update Manager] Error: Inactive partition is invalid.\n");
        return false;
    }

    if (!CRC_Validate(PARTITION_INACTIVE_START, total_written_bytes)) {
        printf("[FW Update Manager] Error: CRC validation failed for inactive firmware.\n");
        return false;
    }

    printf("[FW Update Manager] Integrity verification successful.\n");
    return true;
}













