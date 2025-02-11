

/*Script - memory_manager.h*/
// by:-  Ashutosh Tiwari



#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_BUFFER_SIZE 1024

// Function declarations
bool MemoryManager_Init(void);
void MemoryManager_Deinit(void);
void* MemoryManager_Allocate(uint32_t size);
void MemoryManager_Free(void* ptr);
void* MemoryManager_GetSharedBuffer(void);
void MemoryManager_ReleaseSharedBuffer(void);
void MemoryManager_Clear(void* buffer, uint32_t size);
bool Memory_Cleanup(void);




#endif // MEMORY_MANAGER_H
