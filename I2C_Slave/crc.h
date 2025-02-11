

/*Script - crc.h*/
// by:-  Ashutosh tiwari



#ifndef CRC_VALIDATOR_H
#define CRC_VALIDATOR_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CRC_SUCCESS = 0,
    CRC_ERR_INVALID_ADDRESS,
    CRC_ERR_FLASH_READ_FAIL,
    CRC_ERR_FLASH_WRITE_FAIL,
    CRC_ERR_CRC_MISMATCH,
    CRC_ERR_INVALID_METADATA
} crc_status_t;

uint32_t CRC_Calculate(const uint8_t* data, uint32_t length);

bool CRC_Validate(uint32_t partition_start_address, uint32_t partition_size);
bool CRC_UpdateExpected(uint32_t partition_start_address, uint32_t partition_size);
bool CRC_Store(uint32_t partition_start_address, uint32_t crc_value);

uint32_t CRC_ReadStored(uint32_t partition_start_address);

#endif // CRC_VALIDATOR_H
