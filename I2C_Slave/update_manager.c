


/*Script - update_manager.c*/
// by:-  Ashutosh tiwari


#include "update_manager.h"
#include "flash_manager.h"
#include "max32660.h"
#include <stdio.h>
#include <string.h>


static bool update_in_progress = false;
static uint32_t firmware_size = 0;
static uint32_t bytes_recieved = 0;
static update_status_t last_status = UPDATE_SUCCESS;


static void SetStatus(update_status_t status) {
	last_status = status;
	if (status != UPDATE_SUCCESS) {
		printf("[Update Manager] Error: %d\n", status);
	}
}

void UpdateManager_Init(void) {
	update_in_progress = false;
	bytes_recieved = 0;
	firmware_size = 0;
	last_status = UPDATE_SUCCESS;
	printf("[Update Manager] Initialized complete.\n");

}

update_status_t UpdateManager_StartUpdate(const uint8_t* fimrware_data, uint32_t size) {
	if (update_in_progress) {
		printf("[Update Manager] Update already in progress.\n");
		return UPDATE_IN_PROGRESS;
	}

	if (size > PARTITION_SIZE) {
		printf("[Update Manager] Firmware size exceeds partition size.\n");
		return UPDATE_ERR_PARTITION_INVALID;
	}

	if (Flash_ErasePage(PARTITION_INACTIVE_START) != FLASH_SUCCESS) {
		return UPDATE_ERR_FLASH_FAIL;
	}

	update_in_progress = true;
	firmware_size = size;
	bytes_recieved = 0;
	printf("[Update Manager] Update process started for %d bytes .\n", firmware_size);
	return UPDATE_IN_PROGRESS;
}

void UpdateManager_CancelUpdate(void) {
	if (!update_in_progress) return;

	update_in_progress = false;
	bytes_recieved = 0;
	firmware_size = 0;
	SetStatus(UPDATE_SUCCESS);
	printf("[Update Manager] Update canceled.\n");
}

update_status_t UpdateManager_FinalizeUpdate(void) {
	if (!update_in_progress) {
		return UPDATE_SUCCESS;
	}

	if (!CRC_Validate(PARTITION_INACTIVE_START, firmware_size)) {
		printf("[Update Manager] CRC validation failed.\n");
		SetStatus(UPDATE_ERR_PARTITION_INVALID);
		return last_status;
	}

	update_in_progress = false;
	SetStatus(UPDATE_SUCCESS);
	printf("[Update Manager] Update finished successfully. Ready to boot new firmware.\n");
	return UPDATE_SUCCESS;
}
