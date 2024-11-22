


/*Script - validator_manager.h*/
// by:-  Ashutosh tiwari



#ifndef VALIDATOR_MANAGER_H
#define VALIDATOR_MANAGER_H


#include <stdint.h>
#include <stdbool.h>


void ValidationManager_Init(void);

bool ValidationManager_ValidateMetadata(uint32_t partition_start_address);
bool ValidationManager_ValidateFirmware(uint32_t partition_start_address);


#endif // VALIDATOR_MANAGER_H

