




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "i2c.h"
#include "i2c_driver.h"

#define SLAVE_I2C_ADDRESS 0x55

// Bootloader commands
#define CMD_PING            0x10
#define CMD_PONG            0x11
#define CMD_START_UPDATE    0x01
#define CMD_ERASE_PARTITION 0x02
#define CMD_PROGRAM_PAGE    0x03
#define CMD_VALIDATE_CRC    0x04
#define CMD_MARK_ACTIVE     0x05
#define CMD_REBOOT          0x06

// Firmware file properties
#define CHUNK_SIZE          1024
#define PARTITION_SIZE      0x1C000  // Match the slave partition size
#define FIRMWARE_PATH       "HelloBL_BL_100ms.bin"  // Path to the firmware file

// Function Prototypes
bool Master_Handshake(void);
bool Master_SendCommand(uint8_t command, const uint8_t* payload, uint32_t payload_size, uint8_t* response);
bool Master_EnterBootloader(void);
bool Master_ErasePartition(void);
bool Master_ProgramFirmware(const char* firmware_path);
bool Master_ValidateCRC(void);
bool Master_MarkActive(void);
bool Master_RebootSlave(void);
void FirmwareUpdateProcess(void);

int main(void) {
    // Initialize I2C master
    I2C_Master_Init(SLAVE_I2C_ADDRESS);
    printf("[Master] I2C Initialized. Starting firmware update process...\n");

    // Step 1: Establish communication with the slave
    if (!Master_Handshake()) {
        printf("[Master] Error: Failed to establish communication with slave.\n");
        return -1;
    }

    // Step 2: Execute firmware update process
    FirmwareUpdateProcess();

    return 0;
}

// Handshake with the slave
bool Master_Handshake(void) {
    uint8_t command = CMD_PING;
    uint8_t response;

    printf("[Master] Sending PING to slave...\n");
    if (Master_SendCommand(command, NULL, 0, &response)) {
        if (response == CMD_PONG) {
            printf("[Master] I2C communication established with slave.\n");
            return true;
        }
        printf("[Master] Unexpected response from slave: 0x%02X\n", response);
    } else {
        printf("[Master] I2C communication failed.\n");
    }
    return false;
}

// Firmware update process
void FirmwareUpdateProcess(void) {
    uint8_t response;

    // Step 1: Enter Bootloader Mode
    if (!Master_EnterBootloader()) {
        printf("[Master] Error: Failed to enter bootloader mode.\n");
        return;
    }

    // Step 2: Start Update
    if (!Master_SendCommand(CMD_START_UPDATE, NULL, 0, &response) || response != 0x00) {
        printf("[Master] Error: Failed to start update.\n");
        return;
    }

    // Step 3: Erase Partition
    if (!Master_ErasePartition()) {
        printf("[Master] Error: Failed to erase partition.\n");
        return;
    }

    // Step 4: Program Firmware
    if (!Master_ProgramFirmware(FIRMWARE_PATH)) {
        printf("[Master] Error: Firmware programming failed.\n");
        return;
    }

    // Step 5: Validate CRC
    if (!Master_ValidateCRC()) {
        printf("[Master] Error: Firmware validation failed.\n");
        return;
    }

    // Step 6: Mark Partition Active
    if (!Master_MarkActive()) {
        printf("[Master] Error: Failed to mark partition as active.\n");
        return;
    }

    // Step 7: Reboot Slave
    if (!Master_RebootSlave()) {
        printf("[Master] Error: Failed to reboot slave.\n");
        return;
    }

    printf("[Master] Firmware update completed successfully!\n");
}

// Master Commands Implementation

bool Master_EnterBootloader(void) {
    uint8_t command[3] = {0x01, 0x00, 0x08};  // Enter bootloader mode
    uint8_t response;

    printf("[Master] Sending enter bootloader command...\n");
    return Master_SendCommand(command[0], &command[1], 2, &response) && response == 0xAA;
}

bool Master_ErasePartition(void) {
    uint8_t response;
    printf("[Master] Sending erase partition command...\n");
    return Master_SendCommand(CMD_ERASE_PARTITION, NULL, 0, &response) && response == 0x00;
}

bool Master_ProgramFirmware(const char* firmware_path) {
    FILE* file = fopen(firmware_path, "rb");
    if (!file) {
        printf("[Master] Error: Unable to open firmware file: %s\n", firmware_path);
        return false;
    }

    uint8_t chunk[CHUNK_SIZE];
    uint32_t offset = 0;
    uint8_t response;

    printf("[Master] Programming firmware...\n");
    while (1) {
        // Read a chunk of data from the firmware file
        size_t bytes_read = fread(chunk, 1, CHUNK_SIZE, file);
        if (bytes_read == 0) break;

        // Log the progress
        printf("[Master] Programming chunk at offset 0x%08X, size %zu...\n", offset, bytes_read);

        // Send the command with the chunk
        if (!Master_SendCommand(CMD_PROGRAM_PAGE, chunk, bytes_read, &response) || response != 0x00) {
            printf("[Master] Error: Programming failed at offset 0x%08X.\n", offset);
            fclose(file);
            return false;
        }

        // Move to the next chunk
        offset += bytes_read;
    }

    fclose(file);
    return true;
}

bool Master_ValidateCRC(void) {
    uint8_t response;
    printf("[Master] Sending validate CRC command...\n");
    return Master_SendCommand(CMD_VALIDATE_CRC, NULL, 0, &response) && response == 0x00;
}

bool Master_MarkActive(void) {
    uint8_t response;
    printf("[Master] Sending mark active partition command...\n");
    return Master_SendCommand(CMD_MARK_ACTIVE, NULL, 0, &response) && response == 0x00;
}

bool Master_RebootSlave(void) {
    uint8_t response;
    printf("[Master] Sending reboot command...\n");
    return Master_SendCommand(CMD_REBOOT, NULL, 0, &response) && response == 0x00;
}

bool Master_SendCommand(uint8_t command, const uint8_t* payload, uint32_t payload_size, uint8_t* response) {
    uint8_t tx_buffer[CHUNK_SIZE + 1];  // Command + Payload
    tx_buffer[0] = command;

    if (payload && payload_size > 0) {
        memcpy(&tx_buffer[1], payload, payload_size);
    }

    for (int attempt = 0; attempt < 3; attempt++) {
        if (I2C_Send(tx_buffer, payload_size + 1) == I2C_SUCCESS) {
            if (I2C_Receive(response, 1) == I2C_SUCCESS) {
                return true;
            }
        }
        printf("[Master] Retrying command: 0x%02X (Attempt %d)\n", command, attempt + 1);
    }

    printf("[Master] Failed to send command: 0x%02X\n", command);
    return false;
}
