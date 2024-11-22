


/*Script - partition_manager.h*/
// by:-  Ashutosh tiwari


/*Script - partition_manager.h*/
// by:- Ashutosh Tiwari

#ifndef PARTITION_MANAGER_H
#define PARTITION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// Partition address
#define PARTITION_ACTIVE_START     0x00004000
#define PARTITION_INACTIVE_START   0x00020000
#define PARTITION_SIZE             0x0001C000

#define PARTITION_FLAG_SPACE_START 0x0003F000
#define PARTITION_FLAG_SPACE_SIZE  0x1000

#define PARTITION_VALID_FLAG             0xA5A5A5A5
#define PARTITION_INVALID_FLAG           0xFFFFFFFF
#define PARTITION_UPDATE_IN_PROGRESS_FLAG 0x55555555

typedef enum {
    PARTITION_ACTIVE = 0,
    PARTITION_INACTIVE
} partition_type_t;

typedef struct {
    uint32_t start_address;
    uint32_t size;
    uint32_t validation_flag;
} partition_t;

// Correct declarations
void Partition_Init(void);
bool Partition_Validate(partition_type_t partition);
bool Partition_SwitchActive(void);
void Partition_MarkRollback(partition_type_t partition);
bool Partition_IsValid(partition_type_t partition);

// Update flag operations
bool Partition_SetUpdateFlag(uint32_t flag);
uint32_t Partition_GetUpdateFlag(void);
bool Partition_ClearUpdateFlag(void);

#endif // PARTITION_MANAGER_H
