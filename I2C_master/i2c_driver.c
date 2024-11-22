


#include "i2c_driver.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>

#define I2C_FREQ        100000
#define I2C_TIMEOUT_MS  1000

static volatile int i2c_async_flag = 0;

// Callback function for asynchronous transactions
static void i2c_callback(mxc_i2c_req_t *req, int error) {
    if (error != E_NO_ERROR) {
        printf("[Master] I2C Callback Error: %d\n", error);
    }
    i2c_async_flag = 1; // Mark transaction as complete
}

// Initialize I2C as Master
void I2C_Master_Init(uint8_t slave_address) {
    int ret = MXC_I2C_Init(MXC_I2C0, 1, 0); // Master mode
    if (ret != E_NO_ERROR) {
        printf("[Master] I2C initialization failed with error code: %d\n", ret);
        return;
    }
    MXC_I2C_SetFrequency(MXC_I2C0, I2C_FREQ); // Set frequency
    printf("[Master] I2C initialized at %d Hz\n", I2C_FREQ);
}

// I2C Send Function for Master
i2c_status_t I2C_Master_Send(uint8_t* data, uint32_t length) {
    mxc_i2c_req_t req = {
        .i2c = MXC_I2C0,
        .addr = SLAVE_I2C_ADDRESS,
        .tx_buf = data,
        .tx_len = length,
        .rx_buf = NULL,
        .rx_len = 0,
        .restart = 0,
        .callback = i2c_callback
    };

    i2c_async_flag = 0;
    int ret = MXC_I2C_MasterTransactionAsync(&req);
    if (ret != E_NO_ERROR) {
        printf("[Master] I2C Send Error: %d\n", ret);
        return I2C_ERROR;
    }

    while (!i2c_async_flag); // Wait for completion
    return I2C_SUCCESS;
}

// I2C Receive Function for Master
i2c_status_t I2C_Master_Receive(uint8_t* buffer, uint32_t length) {
    mxc_i2c_req_t req = {
        .i2c = MXC_I2C0,
        .addr = SLAVE_I2C_ADDRESS,
        .tx_buf = NULL,
        .tx_len = 0,
        .rx_buf = buffer,
        .rx_len = length,
        .restart = 0,
        .callback = i2c_callback
    };

    i2c_async_flag = 0;
    int ret = MXC_I2C_MasterTransactionAsync(&req);
    if (ret != E_NO_ERROR) {
        printf("[Master] I2C Receive Error: %d\n", ret);
        return I2C_ERROR;
    }

    while (!i2c_async_flag); // Wait for completion
    return I2C_SUCCESS;
}

