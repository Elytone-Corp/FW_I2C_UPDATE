/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file        main.c
 * @brief       I2C Master-Slave Transaction
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bootloader.h"
#include "log_manager.h"
#include "reboot_manager.h"
#include "state_manager.h"
#include "i2c_driver.h"
#include "flash_manager.h"
#include "partition_manager.h"
#include "command_handler.h"

// I2C slave address (matches the master configuration)
#define SLAVE_I2C_ADDRESS 0x55

// I2C receive buffer size
#define I2C_BUFFER_SIZE 1024

// Simulate a valid firmware (for testing purposes)
void SimulateValidFirmware(void) {
    uint32_t valid_flag = PARTITION_VALID_FLAG;

    printf("[Simulation] Marking inactive partition as valid...\n");

    if (Flash_ProgramPage(PARTITION_INACTIVE_START + PARTITION_SIZE - sizeof(valid_flag),
                          (const uint8_t*)&valid_flag, sizeof(valid_flag)) != FLASH_SUCCESS) {
        printf("[Simulation] Failed to simulate valid firmware.\n");
    } else {
        printf("[Simulation] Inactive partition marked as valid.\n");
    }
}

// I2C command handler (slave-side)
void ProcessI2CCommands(void) {
    uint8_t buffer[I2C_BUFFER_SIZE];
    uint8_t command;
    uint8_t payload[I2C_BUFFER_SIZE - 1];  // Payload excludes the command byte
    uint8_t response = CMD_SUCCESS;
    uint32_t payload_size;

    // Listen for incoming commands from the master
    while (1) {
        printf("[Slave] Waiting for I2C command...\n");
        if (I2C_Receive(buffer, sizeof(buffer)) == I2C_SUCCESS) {
            command = buffer[0];  // First byte is the command
            payload_size = sizeof(buffer) - 1;  // Remaining bytes are the payload

            // Copy payload
            memcpy(payload, &buffer[1], payload_size);

            printf("[Slave] Received command: 0x%02X with payload size: %u\n", command, payload_size);

            // Process the command
            response = CommandHandler_ProcessCommand(command, payload, payload_size);

            // Send response back to master
            if (I2C_Send(&response, sizeof(response)) != I2C_SUCCESS) {
                printf("[Slave] Failed to send response to master.\n");
            } else {
                printf("[Slave] Response sent: 0x%02X\n", response);
            }
        } else {
            printf("[Slave] Error: Failed to receive I2C data.\n");
        }
    }
}

int main(void) {
    printf("[System] Bootloader starting...\n");

    // Initialize Logger
    LogManager_Init();

    // Optionally simulate valid firmware for testing
    SimulateValidFirmware();

    // Initialize Bootloader
    Bootloader_Init();

    // Initialize I2C Slave
    I2C_Slave_Init();
    printf("[Slave] I2C initialized with address: 0x%02X\n", SLAVE_I2C_ADDRESS);

    // Retrieve current bootloader state
    bootloader_state_t state = Bootloader_GetState();

    while (1) {
        switch (state) {
            case BOOTLOADER_UPDATE:
                LogManager_Log(LOG_LEVEL_INFO, "[System] Update process in progress...");
                ProcessI2CCommands();  // Handle update process
                break;

            case BOOTLOADER_BOOT:
                LogManager_Log(LOG_LEVEL_INFO, "[System] Booting firmware...");
                if (!Bootloader_ValidateAndBoot()) {
                    LogManager_Log(LOG_LEVEL_ERROR, "[System] Boot failed. Initiating rollback...");
                    Bootloader_Rollback();
                }
                break;

            case BOOTLOADER_ROLLBACK:
                LogManager_Log(LOG_LEVEL_WARNING, "[System] Rollback state detected, performing rollback...");
                Bootloader_Rollback();
                break;

            case BOOTLOADER_IDLE:
                LogManager_Log(LOG_LEVEL_INFO, "[System] No valid firmware. Waiting for firmware update...");
                while (StateManager_IsInState(BOOTLOADER_IDLE)) {
                    ProcessI2CCommands();  // Wait for firmware updates from master
                }
                break;

            case BOOTLOADER_ERROR:
            default:
                LogManager_Log(LOG_LEVEL_ERROR, "[System] Bootloader encountered a critical error: %d", Bootloader_GetLastError());
                RebootManager_PrepareReboot("Critical Error Detected");
                RebootManager_Reboot();
                break;
        }

        // Update state if necessary
        state = Bootloader_GetState();
    }

    return 0;
}






























































