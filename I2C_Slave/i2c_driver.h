

/*Script - i2c_driver.h*/
// by:-  Ashutosh tiwari



#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// I2C Status
typedef enum {
    I2C_SUCCESS = 0,
    I2C_ERROR,
    I2C_TIMEOUT
} i2c_status_t;

// Public API
void I2C_Slave_Init(void);
i2c_status_t I2C_Send(uint8_t* data, uint32_t length);
i2c_status_t I2C_Receive(uint8_t* buffer, uint32_t length);

#endif // I2C_DRIVER_H
