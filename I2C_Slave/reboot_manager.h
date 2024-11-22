

/*Script - reboot_manager.h*/
// by:-  Ashutosh tiwari


#ifndef REBOOT_MANAGER_H
#define REBOOT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

void RebootManager_Init(void);

void RebootManager_PrepareReboot(const char* reason);

void RebootManager_Reboot(void);

#endif // REBOOT_MANAGER_H




