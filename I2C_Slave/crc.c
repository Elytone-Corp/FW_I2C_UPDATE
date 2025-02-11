

/*Script - crc.c*/
// by:-  Ashutosh tiwari


#include "crc.h"
#include "flash_manager.h"
#include "partition_manager.h"
#include "memory_manager.h"
#include <stdio.h>
#include <string.h>

#define CRC_METADATA_OFFSET (PARTITION_SIZE - CRC_METADATA_SIZE)
#define CRC_POLYNOMIAL      0x04C11DB7  // CRC polynomial
#define INITIAL_CRC_VALUE   0xFFFFFFFF
#define VALID_MARK          0x4D41524B
#define CHUNK_SIZE          512
#define CRC_METADATA_SIZE   8

uint32_t CRC_Calculate(const uint8_t* data, uint32_t length) {
    uint32_t crc = INITIAL_CRC_VALUE;

    for (uint32_t i = 0; i < length; i++) {
        crc ^= ((uint32_t)data[i] << 24);
        for (uint8_t j = 0; j < 8; j++) {
            crc = (crc & 0x80000000) ? (crc << 1) ^ CRC_POLYNOMIAL : (crc << 1);
        }
    }

    return ~crc;
}

bool CRC_Validate(uint32_t partition_start_address, uint32_t partition_size) {
    printf("[CRC Validator] Validating firmware CRC at 0x%08X...\n", partition_start_address);

    uint32_t calculated_crc = INITIAL_CRC_VALUE;
    uint8_t* crc_buffer = MemoryManager_Allocate(CHUNK_SIZE);
    if (!crc_buffer) {
        printf("[CRC Validator] Buffer allocation failed for CRC validation.\n");
        return false;
    }

    uint32_t remaining_size = partition_size - CRC_METADATA_SIZE;
    uint32_t current_address = partition_start_address;

    // Incrementally calculate CRC
    while (remaining_size > 0) {
        uint32_t chunk_size = (remaining_size > CHUNK_SIZE) ? CHUNK_SIZE : remaining_size;

        // Copy data from partition to buffer for CRC calculation
        memcpy(crc_buffer, (const void*)current_address, chunk_size);
        calculated_crc = CRC_Calculate(crc_buffer, chunk_size);

        current_address += chunk_size;
        remaining_size -= chunk_size;
    }

    MemoryManager_Free(crc_buffer);

    calculated_crc = ~calculated_crc;

    uint32_t stored_crc = CRC_ReadStored(partition_start_address);

    if (calculated_crc == stored_crc) {
        printf("[CRC Validator] CRC validation successful.\n");
        return true;
    } else {
        printf("[CRC Validator] CRC mismatch! Calculated: 0x%08X, Stored: 0x%08X\n", calculated_crc, stored_crc);
        return false;
    }
}

uint32_t CRC_ReadStored(uint32_t partition_start_address) {
    return *(volatile uint32_t*)(partition_start_address + CRC_METADATA_OFFSET);
}

uint32_t CRC_ReadValidMark(uint32_t partition_start_address) {
    return *(volatile uint32_t*)(partition_start_address + CRC_METADATA_OFFSET + 4);
}

bool CRC_Store(uint32_t partition_start_address, uint32_t crc_value) {
    printf("[CRC Validator] Storing CRC value at 0x%08X: 0x%08X\n", partition_start_address + CRC_METADATA_OFFSET, crc_value);

    if (Flash_ProgramPage(partition_start_address + CRC_METADATA_OFFSET, (const uint8_t*)&crc_value, sizeof(crc_value)) != FLASH_SUCCESS) {
        printf("[CRC Validator] Error storing CRC value.\n");
        return false;
    }

    uint32_t valid_mark = VALID_MARK;
    if (Flash_ProgramPage(partition_start_address + CRC_METADATA_OFFSET + 4, (const uint8_t*)&valid_mark, sizeof(valid_mark)) != FLASH_SUCCESS) {
        printf("[CRC Validator] Error storing valid mark.\n");
        return false;
    }

    return true;
}

bool CRC_UpdateExpected(uint32_t partition_start_address, uint32_t partition_size) {
    printf("[CRC Validator] Updating expected CRC for partition at 0x%08X...\n", partition_start_address);
    uint32_t calculated_crc = CRC_Calculate((const uint8_t*)partition_start_address, partition_size - CRC_METADATA_SIZE);
    return CRC_Store(partition_start_address, calculated_crc);
}

bool CRC_CheckValidMark(uint32_t partition_start_address) {
    uint32_t valid_mark = CRC_ReadValidMark(partition_start_address);

    if (valid_mark == VALID_MARK) {
        printf("[CRC Validator] Valid mark is present.\n");
        return true;
    } else {
        printf("[CRC Validator] Valid mark is missing.\n");
        return false;
    }
}


