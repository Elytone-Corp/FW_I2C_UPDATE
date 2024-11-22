


/*Script - command_handler.h*/
// by:-  Ashutosh tiwari



#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

// Command Definitions
#define CMD_START_UPDATE     0x01
#define CMD_ERASE_PARTITION  0x02
#define CMD_PROGRAM_PAGE     0x03
#define CMD_VALIDATE_CRC     0x04
#define CMD_MARK_ACTIVE      0x05
#define CMD_REBOOT           0x06
#define CMD_QUERY_STATUS     0x07  // New command for querying status

// Response Codes
typedef enum {
    CMD_SUCCESS = 0,
    CMD_ERR_INVALID_COMMAND,
    CMD_ERR_FLASH_OP_FAILED,
    CMD_ERR_CRC_MISMATCH,
    CMD_ERR_PARTITION_SWITCH_FAIL,
    CMD_ERR_PAYLOAD_SIZE_EXCEEDED,
    CMD_ERR_UNKNOWN
} cmd_response_t;

// Function Prototypes
void CommandHandler_Init(void);
cmd_response_t CommandHandler_ProcessCommand(uint8_t command, const uint8_t* payload, uint32_t payload_length);

#endif  // COMMAND_HANDLER_H

