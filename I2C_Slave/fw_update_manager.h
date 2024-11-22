


/*Script -- fw_update_manager.h*/
// Edited by :-  Ashutosh Tiwari



#ifndef FW_UPDATE_MANAGER_H
#define FW_UPDATE_MANAGER_H


#include <stdint.h>
#include <stdbool.h>
#include "command_handler.h"


void FWUpdateManger_Init(void);

cmd_response_t FWUpdateManager_StartUpdate(void);

cmd_response_t FWUpdateManger_FinalizeUpdate(void);

void FWUpdateManager_CancelUpdate(void);

#endif // FW_UPDATE_MANAGER

