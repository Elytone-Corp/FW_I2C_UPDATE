


/*Script - update_manager.h*/
// by:-  Ashutosh tiwari


#ifndef UPDATE_MANAGER_H
#define UPDATE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "command_handler.h"
#include "fw_update_manager.h"
#include "partition_manager.h"
#include "crc.h"
#include "state_manager.h"
#include "bootloader.h"


typedef enum {
	UPDATE_SUCCESS = 0,
	UPDATE_IN_PROGRESS,
	UPDATE_FAILED,
	UPDATE_CANCELED,
	UPDATE_ERR_PARTITION_INVALID,
	UPDATE_ERR_FLASH_FAIL,
	UPDATE_ERR_CRC_MISMATCH
} update_status_t;

void UpadateManager_Init(void);

update_status_t UpdatManager_StartUpdate(const uint8_t* firmware_data, uint32_t firmware_size);

void UpdateManager_CancelUpdate(void);

update_status_t UpdateManager_FinalizeUpdate(void);

#endif // UPDATE_MANAGER_H
