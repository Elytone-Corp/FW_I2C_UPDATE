

/*Script -- communication_manager.h*/
// Edited by :-  Ashutosh Tiwari




#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PACKET_SIZE 256
#define COMM_ACK 0xA0
#define COMM_NACK 0xA6
#define COMM_RETRY_LIMIT 3


typedef enum {
	COMM_SUCCESS = 0,
	COMM_BUFFER_SIZE,
	COMM_ERR_INVALID_PACKET,
	COMM_ERR_TIMEOUT,
	COMM_ERR_TRANSMISSION_FAIL,
	COMM_ERR_CRC_FAIL
} comm_status_t;


void CommunicationManager_Init(void);

comm_status_t CommunicationManager_Send(const uint8_t* packet, uint32_t length);
comm_status_t CommunicationManager_Recieve(uint8_t* buffer, uint32_t* length);

bool CommunicationManager_ValidatePacket(const uint8_t* packet, uint32_t lenght);

#endif //COMMUNICATION_MANAGER_H

