


/*Script - state_manager.h*/
// by:-  Ashutosh tiwari


#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "bootloader.h"


extern bootloader_state_t current_state;

void StateManager_Init(void);

void StateManager_SetState(bootloader_state_t state);

bootloader_state_t StateManager_GetState(void);

bool StateManager_IsInState(bootloader_state_t state);

#endif // STATE_MANAGER_H



