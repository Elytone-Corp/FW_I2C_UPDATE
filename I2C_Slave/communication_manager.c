

/*Script -- communication_manager.c*/
// Edited by :-  Ashutosh Tiwari



#include "communication_manager.h"
#include "memory_manager.h"
#include "crc.h"
#include "state_manager.h"
#include <string.h>
#include <stdio.h>

static uint8_t retry_count = 0;
static uint8_t* comm_buffer = NULL;

void CommunicationManager_Init(void) {
    printf("[Communication Manager] Initializing communication....\n");
    retry_count = 0;

    comm_buffer = MemoryManager_Allocate(COMM_BUFFER_SIZE);
    if (!comm_buffer) {
        printf("[CommManager] Buffer allocation failed.\n");
        return;
    }

    printf("[CommManager] Initialization complete.\n");
}

void CommunicationManager_Deinit(void) {
    if (comm_buffer) {
        MemoryManager_Free(comm_buffer); // Corrected function call
        comm_buffer = NULL;
    }
}

comm_status_t CommunicationManager_Send(const uint8_t* packet, uint32_t length) {
    if (length > MAX_PACKET_SIZE) {
        printf("[Communication Manager] Error: Packet size exceeds limit.\n");
        return COMM_ERR_INVALID_PACKET;
    }

    printf("[Communication Manager] Sending packet of length %d....\n", length);
    uint8_t response = COMM_NACK;

    for (retry_count = 0; retry_count < COMM_RETRY_LIMIT; retry_count++) {
        // Simulate send logic here.
        response = COMM_ACK;  // Replace this with actual communication logic
        if (response == COMM_ACK) {
            printf("[Communication Manager] Packet sent successfully.\n");
            return COMM_SUCCESS;
        }
    }

    printf("[Communication Manager] Transmission failed after %d retries.\n", COMM_RETRY_LIMIT);
    return COMM_ERR_TRANSMISSION_FAIL;
}

comm_status_t CommunicationManager_Receive(uint8_t* buffer, uint32_t* length) {
    printf("[Communication Manager] Waiting for incoming data...\n");

    uint8_t received_packet[MAX_PACKET_SIZE] = {0};  // Mock received packet
    uint32_t received_length = 100;                  // Mock packet length

    memcpy(buffer, received_packet, received_length);
    *length = received_length;

    if (!CommunicationManager_ValidatePacket(buffer, *length)) {
        printf("[CommunicationManager] Received packet failed CRC validation.\n");
        return COMM_ERR_CRC_FAIL;
    }

    printf("[CommunicationManager] Packet received successfully.\n");
    return COMM_SUCCESS;
}

bool CommunicationManager_ValidatePacket(const uint8_t* packet, uint32_t length) {
    uint32_t calculated_crc = CRC_Calculate(packet, length - sizeof(uint32_t));
    uint32_t received_crc = *(uint32_t*)(packet + length - sizeof(uint32_t));

    if (calculated_crc != received_crc) {
        printf("[Communication Manager] CRC mismatch! Calculated: 0x%08X, Received: 0x%08X\n", calculated_crc, received_crc);
        return false;
    }

    return true;
}

