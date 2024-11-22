

/*Script - i2c_driver.c*/
// by:-  Ashutosh tiwari



#include "i2c_driver.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>

#define I2C_FREQ        100000
#define I2C_SLAVE_ADDR  0x55

static volatile int i2c_event_flag = 0;

// Callback function for slave events
static int i2c_slave_callback(mxc_i2c_regs_t *i2c, mxc_i2c_slave_event_t event, void *data) {
    switch (event) {
        case MXC_I2C_EVT_TRANS_COMP:
            printf("[Slave] Transaction Complete\n");
            i2c_event_flag = 1; // Mark transaction as complete
            break;
        default:
            break;
    }
    return 0; // Acknowledge the transaction
}

// Initialize I2C as Slave
void I2C_Slave_Init(uint8_t slave_address) {
    int ret = MXC_I2C_Init(MXC_I2C1, 0, slave_address); // Slave mode on MXC_I2C1
    if (ret != E_NO_ERROR) {
        printf("[Slave] I2C initialization failed with error code: %d\n", ret);
        return;
    }
    printf("[Slave] I2C initialized with address: 0x%02X\n", slave_address);
}

// I2C Receive Function for Slave
i2c_status_t I2C_Slave_Receive(uint8_t* buffer, uint32_t length) {
    int ret = MXC_I2C_SlaveTransaction(MXC_I2C1, i2c_slave_callback); // Blocking receive
    if (ret != E_NO_ERROR) {
        printf("[Slave] I2C Receive Error: %d\n", ret);
        return I2C_ERROR;
    }
    MXC_I2C_ReadRXFIFO(MXC_I2C1, buffer, length);
    return I2C_SUCCESS;
}

// I2C Send Function for Slave
i2c_status_t I2C_Slave_Send(uint8_t* data, uint32_t length) {
    int ret = MXC_I2C_WriteTXFIFO(MXC_I2C1, data, length);
    if (ret != length) {
        printf("[Slave] I2C Send Error\n");
        return I2C_ERROR;
    }
    return I2C_SUCCESS;
}

