



/*Script - validation_manager.c*/
// by:-  Ashutosh Tiwari



#include "validation_manager.h"
#include "crc.h"
#include "partition_manager.h"
#include "max32660.h"
#include <stdio.h>
#include <string.h>

#define MAGIC_NUMBER 0x4D41524B
#define METADATA_OFFSET (PARTITION_SIZE - 16)

typedef struct {
    uint32_t magic_number;
    uint32_t version;
    uint32_t crc;
} firmware_metadata_t;


static firmware_metadata_t ValidationManager_ReadMetadata(uint32_t partition_start_address) {
    firmware_metadata_t metadata;
    memcpy(&metadata, (void*)(partition_start_address + METADATA_OFFSET), sizeof(metadata));
    return metadata;
}

void ValidationManager_Init(void) {
    printf("[ValidationManager] Initialized.\n");
}

bool ValidationManager_ValidateActiveFirmware(void) {
    return ValidationManager_ValidateFirmware(PARTITION_ACTIVE_START);
}

bool ValidationManager_ValidateFirmware(uint32_t partition_start_address) {
    printf("[ValidationManager] Validating firmware at 0x%08X...\n", partition_start_address);

    if (!ValidationManager_ValidateMetadata(partition_start_address)) {
        printf("[ValidationManager] Metadata validation failed.\n");
        return false;
    }

    if (!CRC_Validate(partition_start_address, PARTITION_SIZE)) {
        printf("[ValidationManager] CRC validation failed.\n");
        return false;
    }

    printf("[ValidationManager] Firmware validation successful.\n");
    return true;
}


bool ValidationManager_ValidateMetadata(uint32_t partition_start_address) {
    firmware_metadata_t metadata = ValidationManager_ReadMetadata(partition_start_address);

    if (metadata.magic_number != MAGIC_NUMBER) {
        printf("[ValidationManager] Invalid magic number: 0x%08X\n", metadata.magic_number);
        return false;
    }

    printf("[ValidationManager] Metadata validation successful.\n");
    return true;
}


