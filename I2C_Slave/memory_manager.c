


/*Script - memory_manager.h*/
// by:-  Ashutosh Tiwari



#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UPDATE_BUFFER_SIZE 4096  // 4 KB buffer size

// Static variables
static uint8_t shared_buffer[MEMORY_BUFFER_SIZE];
static bool shared_buffer_in_use = false;
static uint8_t* update_buffer = NULL;

// Initialize Memory Manager
bool MemoryManager_Init(void) {
    printf("[Memory Manager] Initialization complete.\n");
    memset(shared_buffer, 0, MEMORY_BUFFER_SIZE);
    shared_buffer_in_use = false;

    return true;
}

// De-initialize Memory Manager
void MemoryManager_Deinit(void) {
    printf("[Memory Manager] De-initialization complete.\n");
    shared_buffer_in_use = false;
}

// Allocate dynamic memory
void* MemoryManager_Allocate(uint32_t size) {
    if (size == 0 || size > MEMORY_BUFFER_SIZE) {
        printf("[Memory Manager] Allocation failed: Invalid size %u.\n", size);
        return NULL;
    }

    void* buffer = malloc(size);
    if (buffer == NULL) {
        printf("[Memory Manager] Allocation failed: insufficient memory.\n");
    } else {
        printf("[Memory Manager] Allocated buffer of size %u.\n", size);
    }

    return buffer;
}

// Free allocated memory
void MemoryManager_Free(void* buffer) {
    if (buffer != NULL) {
        free(buffer);
        printf("[Memory Manager] Buffer freed.\n");
    } else {
        printf("[Memory Manager] Invalid buffer for freeing.\n");
    }
}

// Get shared buffer
void* MemoryManager_GetSharedBuffer(void) {
    if (shared_buffer_in_use) {
        printf("[Memory Manager] Shared buffer is currently in use.\n");
        return NULL;
    }

    shared_buffer_in_use = true;
    printf("[Memory Manager] Shared buffer allocated.\n");
    return shared_buffer;
}

// Release shared buffer
void MemoryManager_ReleaseSharedBuffer(void) {
    shared_buffer_in_use = false;
    printf("[Memory Manager] Shared buffer released.\n");
}

// Firmware update buffer management (optional utility)
bool UpdateManager_StartFirmwareUpdate(void) {
    update_buffer = MemoryManager_Allocate(UPDATE_BUFFER_SIZE);
    if (!update_buffer) {
        printf("[UpdateManager] Failed to allocate update buffer.\n");
        return false;
    }

    printf("[UpdateManager] Firmware update started.\n");
    return true;
}

void UpdateManager_EndFirmwareUpdate(void) {
    MemoryManager_Free(update_buffer);
    update_buffer = NULL;
    printf("[UpdateManager] Firmware update completed.\n");
}


bool Memory_Cleanup(void) {
    // Perform cleanup tasks if required
    return true;  // Return success
}
