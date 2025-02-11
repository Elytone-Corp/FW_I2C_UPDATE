

/* Script - partition_manager.c */
// by:- Ashutosh Tiwari

#include "partition_manager.h"
#include "flash_manager.h"
#include "max32660.h"
#include <string.h>
#include <stdio.h>

#define PARTITION_UPDATE_IN_PROGRESS  0xA5A5A5A5
#define PARTITION_UPDATE_COMPLETE     0x5A5A5A5A

static partition_t active_partition = {
    .start_address = PARTITION_ACTIVE_START,
    .size = PARTITION_SIZE,
    .validation_flag = PARTITION_INVALID_FLAG
};

static partition_t inactive_partition = {
    .start_address = PARTITION_INACTIVE_START,
    .size = PARTITION_SIZE,
    .validation_flag = PARTITION_INVALID_FLAG
};

uint32_t flag = PARTITION_UPDATE_IN_PROGRESS;

void Partition_MarkUpdateInProgress(void) {
	Flash_ProgramPage(PARTITION_FLAG_SPACE_START, (const uint8_t*)&flag, sizeof(flag));
}

uint32_t flag_complete = PARTITION_UPDATE_COMPLETE;
void Partition_MarkUpdateComplete(void) {
	Flash_ProgramPage(PARTITION_FLAG_SPACE_START, (const uint8_t*)&flag_complete, sizeof(flag_complete));
}

void Partition_Init(void) {
    printf("Initializing Partition....\n");

    active_partition.validation_flag = *(volatile uint32_t*)(active_partition.start_address + PARTITION_SIZE - 4);
    inactive_partition.validation_flag = *(volatile uint32_t*)(inactive_partition.start_address + PARTITION_SIZE - 4);

    printf("Active Partition: %s\n", Partition_IsValid(PARTITION_ACTIVE) ? "Valid" : "Invalid");
    printf("Inactive Partition: %s\n", Partition_IsValid(PARTITION_INACTIVE) ? "Valid" : "Invalid");
}

bool Partition_Validate(partition_type_t partition) {
    partition_t *target_partition = (partition == PARTITION_ACTIVE) ? &active_partition : &inactive_partition;

    printf("Validating Partition at Address : 0x%08X\n", target_partition->start_address);

    if (target_partition->validation_flag == PARTITION_VALID_FLAG) {
        printf("Partition is valid.\n");
        return true;
    }

    printf("Partition is Invalid.\n");
    return false;
}

bool Partition_SwitchActive(void) {
    printf("Switch Active Partition...\n");

    if (!Partition_Validate(PARTITION_INACTIVE)) {
        printf("Inactive partition is not valid. Aborting switch.\n");
        return false;
    }

    active_partition.validation_flag = PARTITION_INVALID_FLAG;
    inactive_partition.validation_flag = PARTITION_VALID_FLAG;

    printf("Active partition switched successfully.\n");
    return true;
}

void Partition_MarkRollback(partition_type_t partition) {
    partition_t *target_partition = (partition == PARTITION_ACTIVE) ? &active_partition : &inactive_partition;

    printf("Marking Partition at 0x%08X for rollback.\n", target_partition->start_address);
    target_partition->validation_flag = PARTITION_INVALID_FLAG;
}

bool Partition_IsValid(partition_type_t partition) {
    partition_t *target_partition = (partition == PARTITION_ACTIVE) ? &active_partition : &inactive_partition;
    return target_partition->validation_flag == PARTITION_VALID_FLAG;
}

bool Partition_SetUpdateFlag(uint32_t flag) {
    printf("[Partition Manager] Setting update flag: 0x%08X\n", flag);
    return Flash_ProgramPage(PARTITION_FLAG_SPACE_START, (const uint8_t*)&flag, sizeof(flag)) == FLASH_SUCCESS;
}

uint32_t Partition_GetUpdateFlag(void) {
    return *(volatile uint32_t*)(PARTITION_FLAG_SPACE_START);
}

bool Partition_ClearUpdateFlag(void) {
    uint32_t clear_flag = PARTITION_INVALID_FLAG;
    printf("[Partition Manager] Clearing update flag...\n");
    return Flash_ProgramPage(PARTITION_FLAG_SPACE_START, (const uint8_t*)&clear_flag, sizeof(clear_flag)) == FLASH_SUCCESS;
}
















