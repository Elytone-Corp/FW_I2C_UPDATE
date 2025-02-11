

/*Script - flash_manager.c*/
// by:-  Ashutosh tiwari


#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

#define INITIAL_BUFFER_SIZE 512

typedef enum {
    BOOTLOADER_IDLE,
    BOOTLOADER_BOOT,
    BOOTLOADER_UPDATE,
    BOOTLOADER_VALIDATE,
    BOOTLOADER_ROLLBACK,
    BOOTLOADER_ERROR
} bootloader_state_t;


typedef enum {
	BOOTLOADER_NO_ERROR = 0,
	BOOTLOADER_ERR_NO_VALID_FW,
	BOOTLOADER_ERR_FW_UPDATE_FAIL,
	BOOTLOADER_ERR_PARTITION_SWITCH_FAIL,
	BOOTLOADER_ERR_CRC_MISMATCH,
	BOOTLOADER_ERR_MEMORY_INIT_FAIL

} bootloader_error_t;

void Bootloader_Init(void);

bool Bootloader_FirmwareUpdate(const uint8_t* firmware, uint32_t size);

bool Bootloader_ValidateAndBoot(void);

void Bootloader_Rollback(void);

bootloader_state_t Bootloader_GetState(void);

bootloader_error_t Bootloader_GetLastError(void);

#endif //BOOTLOADER_CORE_H



