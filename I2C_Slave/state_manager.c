


/*Script - state_manager.h*/
// by:-  Ashutosh tiwari


#include "state_manager.h"
#include <stdio.h>

// Define the global variable
bootloader_state_t current_state = BOOTLOADER_BOOT;

void StateManager_Init(void) {
    current_state = BOOTLOADER_BOOT;
    printf("[State Manager] Initialized to BOOTLOADER_BOOT.\n");
}

void StateManager_SetState(bootloader_state_t state) {
    if (state < BOOTLOADER_IDLE || state > BOOTLOADER_ERROR) {
        printf("[State Manager] Invalid state transition attempted.\n");
        return;
    }

    printf("[State Manager] Transitioning to state: %d\n", state);
    current_state = state;
}

bootloader_state_t StateManager_GetState(void) {
    return current_state;
}

bool StateManager_IsInState(bootloader_state_t state) {
    return current_state == state;
}
