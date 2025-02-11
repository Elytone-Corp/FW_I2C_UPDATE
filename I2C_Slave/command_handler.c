

/*Script - command_handler.c*/
// by:-  Ashutosh tiwari


#include "command_handler.h"
#include "flash_manager.h"
#include "log_manager.h"
#include "crc.h"
#include "partition_manager.h"
#include "bootloader.h"
#include <stdio.h>
#include <string.h>

// Offset to keep track of the write position in the partition
static uint32_t program_offset = 0;

// Initialization function
void CommandHandler_Init(void) {
    program_offset = 0;  // Initialize program offset to 0
    LogManager_Log(LOG_LEVEL_INFO, "[CommandHandler] Initialization complete.\n");
}

// Process incoming commands
cmd_response_t CommandHandler_ProcessCommand(uint8_t command, const uint8_t* payload, uint32_t payload_length) {
    switch (command) {
        case CMD_START_UPDATE:
            Logger_Info("[CommandHandler] CMD_START_UPDATE received.\n");
            Partition_Init();
            program_offset = 0;  // Reset the offset for programming
            return CMD_SUCCESS;

        case CMD_ERASE_PARTITION:
            Logger_Info("[CommandHandler] CMD_ERASE_PARTITION received.\n");
            if (Flash_ErasePage(PARTITION_INACTIVE_START) == FLASH_SUCCESS) {
                Logger_Info("[CommandHandler] Partition erased successfully.\n");
                return CMD_SUCCESS;
            } else {
                Logger_Info("[CommandHandler] Partition erase failed.\n");
                return CMD_ERR_FLASH_OP_FAILED;
            }

        case CMD_PROGRAM_PAGE:
            Logger_Info("[CommandHandler] CMD_PROGRAM_PAGE received.\n");

            // Validate payload size
            if (payload_length == 0 || payload_length > PARTITION_SIZE) {
            	Logger_Info("[CommandHandler] Error: Invalid payload size.\n");
                return CMD_ERR_PAYLOAD_SIZE_EXCEEDED;
            }

            // Calculate the address to write to
            uint32_t write_address = PARTITION_INACTIVE_START + program_offset;
            Logger_Info("[CommandHandler] Writing to address: 0x%08X\n", write_address);

            // Write the payload to flash
            if (Flash_ProgramPage(write_address, payload, payload_length) == FLASH_SUCCESS) {
                Logger_Info("[CommandHandler] Chunk written at offset 0x%08X.\n", program_offset);
                program_offset += payload_length;  // Update offset for next chunk
                return CMD_SUCCESS;
            } else {
            	Logger_Info("[CommandHandler] Flash write failed at offset 0x%08X.\n", program_offset);
                return CMD_ERR_FLASH_OP_FAILED;
            }

        case CMD_VALIDATE_CRC:
            Logger_Info("[CommandHandler] CMD_VALIDATE_CRC received.\n");
            if (CRC_Validate(PARTITION_INACTIVE_START, program_offset)) {
                Logger_Info("[CommandHandler] CRC validation successful.\n");
                return CMD_SUCCESS;
            } else {
            	Logger_Info("[CommandHandler] CRC validation failed.\n");
                return CMD_ERR_CRC_MISMATCH;
            }

        case CMD_MARK_ACTIVE:
            Logger_Info("[CommandHandler] CMD_MARK_ACTIVE received.\n");
            if (Partition_SwitchActive()) {
                Logger_Info("[CommandHandler] Partition marked as active.\n");
                return CMD_SUCCESS;
            } else {
                Logger_Info("[CommandHandler] Failed to mark partition as active.\n");
                return CMD_ERR_PARTITION_SWITCH_FAIL;
            }

        case CMD_REBOOT:
            Logger_Info("[CommandHandler] CMD_REBOOT received.\n");
            Bootloader_Rollback();
            return CMD_SUCCESS;

        case CMD_QUERY_STATUS:
            Logger_Info("[CommandHandler] CMD_QUERY_STATUS received.\n");
            bootloader_state_t state = Bootloader_GetState();
            bootloader_error_t error = Bootloader_GetLastError();
            Logger_Info("[CommandHandler] Current State: %d, Last Error: %d\n", state, error);
            return CMD_SUCCESS;

        default:
            Logger_Info("[CommandHandler] Unknown command received: 0x%02X\n", command);
            return CMD_ERR_INVALID_COMMAND;
    }
}


