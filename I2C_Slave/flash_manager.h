

/*Script -- flash_manager.h*/
// Edited by :-  Ashutosh Tiwari




#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// Flash memory configuration
#define FLASH_TOTAL_SIZE      0x40000  // 256KB
#define FLASH_PAGE_SIZE       0x2000   // 8KB per page

// Flash partitions
#define FLASH_BOOTLOADER_START   0x00000000
#define FLASH_BOOTLOADER_SIZE    0x00004000  // 16KB
#define FLASH_BANK0_START        0x00004000
#define FLASH_BANK0_SIZE         0x0001C000  // 112KB
#define FLASH_BANK1_START        0x00020000
#define FLASH_BANK1_SIZE         0x0001C000  // 112KB
#define FLASH_FLAG_SPACE_START   0x0003F000
#define FLASH_FLAG_SPACE_SIZE    0x00001000  // 4KB

typedef enum {
    FLASH_SUCCESS = 0,
    FLASH_ERROR_INVALID_ADDRESS,
    FLASH_ERROR_WRITE,
    FLASH_ERROR_ERASE,
    FLASH_ERROR_PROTECTION
} flash_status_t;

// Public API functions
void Flash_Init(void);
flash_status_t Flash_ErasePage(uint32_t address);
flash_status_t Flash_ProgramPage(uint32_t address, const uint8_t *data, uint32_t length);
bool Flash_VerifyPage(uint32_t address, const uint8_t *data, uint32_t length);
bool Flash_IsAddressValid(uint32_t address);
void Flash_ProtectBootloader(void);
void Flash_UnprotectAll(void);

// Missing Function Declarations
int MXC_FLC_WriteProtectionEnable(uint32_t start_addr, uint32_t size);
int MXC_FLC_WriteProtectionDisable(void);
int MXC_FLC_SetPageWriteProtection(uint32_t address, bool protect);

#endif // FLASH_MANAGER_H

