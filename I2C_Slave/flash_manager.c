

/*Script - flash_manager.c*/
// by:-  Ashutosh tiwari


#include "flash_manager.h"
#include "mxc_device.h"
#include "memory_manager.h"
#include "mxc_delay.h"
#include "flc.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Macros
#define MAX_RETRY_COUNT 3
#define PAGE_START_ADDR(addr)  ((addr) & ~(FLASH_PAGE_SIZE - 1))

#define FLASH_SUCCESS 0

static void Flash_LogError(const char* message, int error_code) {
    printf("Flash Error: %s (Code: %d)\n", message, error_code);
}

void Flash_Init(void) {
    int ret = MXC_FLC_Init();
    if (ret != E_NO_ERROR) {
        Flash_LogError("Failed to initialize Flash Controller", ret);

        for (int attempt = 1; attempt <= MAX_RETRY_COUNT; attempt++) {
            ret = MXC_FLC_Init();
            if (ret == E_NO_ERROR) {
                printf("Flash Controller initialized on retry %d\n", attempt);
                return;
            }
            Flash_LogError("Retrying Flash Controller initialization", ret);
            MXC_Delay(MXC_DELAY_MSEC(50));
        }

        printf("Critical: Flash Controller initialization failed after %d attempts\n", MAX_RETRY_COUNT);
    }
}

flash_status_t Flash_ErasePage(uint32_t address) {
    if (!Flash_IsAddressValid(address)) {
        Flash_LogError("Invalid address for page erase", FLASH_ERROR_INVALID_ADDRESS);
        return FLASH_ERROR_INVALID_ADDRESS;
    }

    int ret = MXC_FLC_PageErase(PAGE_START_ADDR(address));
    if (ret != E_NO_ERROR) {
        Flash_LogError("Failed to erase flash page", ret);
        return FLASH_ERROR_ERASE;
    }

    return FLASH_SUCCESS;
}

flash_status_t Flash_ProgramPage(uint32_t address, const uint8_t *data, uint32_t length) {
    if (!Flash_IsAddressValid(address) || (length > FLASH_PAGE_SIZE)) {
        Flash_LogError("Invalid address or data length for programming", FLASH_ERROR_INVALID_ADDRESS);
        return FLASH_ERROR_INVALID_ADDRESS;
    }

    flash_status_t erase_status = Flash_ErasePage(address);
    if (erase_status != FLASH_SUCCESS) {
        return erase_status;
    }

    for (int attempt = 1; attempt <= MAX_RETRY_COUNT; attempt++) {
        // Cast the data pointer to match the expected argument type.
        int ret = MXC_FLC_Write(address, length, (uint32_t*)data);
        if (ret == E_NO_ERROR && Flash_VerifyPage(address, data, length)) {
            return FLASH_SUCCESS;
        }

        Flash_LogError("Flash programming failed, retrying", ret);
        MXC_Delay(MXC_DELAY_MSEC(50));
    }

    Flash_LogError("Critical: Flash programming failed after multiple attempts", FLASH_ERROR_WRITE);
    return FLASH_ERROR_WRITE;
}

bool Flash_VerifyPage(uint32_t address, const uint8_t *data, uint32_t length) {
    if (!Flash_IsAddressValid(address)) {
        Flash_LogError("Invalid address for verification", FLASH_ERROR_INVALID_ADDRESS);
        return false;
    }

    const uint8_t *flash_data = (const uint8_t *)address;
    return (memcmp(flash_data, data, length) == 0);
}

bool Flash_IsAddressValid(uint32_t address) {
    return ((address >= FLASH_BOOTLOADER_START && address < FLASH_TOTAL_SIZE) &&
            (address % 4 == 0));
}

void Flash_ProtectBootloader(void) {
    int ret = MXC_FLC_WriteProtectionEnable(FLASH_BOOTLOADER_START, FLASH_BOOTLOADER_SIZE);
    if (ret != E_NO_ERROR) {
        Flash_LogError("Failed to protect bootloader", ret);
    }
}

void Flash_UnprotectAll(void) {
    int ret = MXC_FLC_WriteProtectionDisable();
    if (ret != E_NO_ERROR) {
        Flash_LogError("Failed to unprotect flash pages", ret);
    }
}

int MXC_FLC_WriteProtectionEnable(uint32_t start_addr, uint32_t size) {
    uint32_t end_addr = start_addr + size - 1;

    if (start_addr % FLASH_PAGE_SIZE != 0 || end_addr >= FLASH_TOTAL_SIZE) {
        return FLASH_ERROR_INVALID_ADDRESS;
    }

    for (uint32_t addr = start_addr; addr <= end_addr; addr += FLASH_PAGE_SIZE) {
        int ret = MXC_FLC_SetPageWriteProtection(addr, true);
        if (ret != E_NO_ERROR) {
            return ret;
        }
    }

    return E_NO_ERROR;
}

int MXC_FLC_WriteProtectionDisable(void) {
    for (uint32_t addr = FLASH_BOOTLOADER_START; addr < FLASH_TOTAL_SIZE; addr += FLASH_PAGE_SIZE) {
        int ret = MXC_FLC_SetPageWriteProtection(addr, false);
        if (ret != E_NO_ERROR) {
            return ret;
        }
    }

    return E_NO_ERROR;
}

int MXC_FLC_SetPageWriteProtection(uint32_t address, bool protect) {

    return E_NO_ERROR;
}









