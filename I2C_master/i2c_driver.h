





#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>
#include "mxc_device.h"

// Slave I2C address
#define SLAVE_I2C_ADDRESS 0x55

// I2C Status Codes
typedef enum {
    I2C_SUCCESS = 0,
    I2C_ERROR
} i2c_status_t;

// Function Prototypes
void I2C_Master_Init(uint8_t slave_address);
void I2C_Master_Deinit(void);
i2c_status_t I2C_Send(const uint8_t* data, uint32_t length);
i2c_status_t I2C_Receive(uint8_t* buffer, uint32_t length);

#endif // I2C_DRIVER_H
