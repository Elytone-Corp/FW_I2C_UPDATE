#include "i2c_slave.h"
#include "i2c_definitions.h"
#include <stdio.h>
#include "mxc_config.h"
#include "i2c.h"
#include "mxc_delay.h"

void i2c_callback(i2c_req_t *req, int error) {
    if (error == E_NO_ERROR && req->rx_num > 0) {
        printf("Data received from master: 0x%02X\n", req->rx_data[0]);
    }
    i2c_slave_callback_init();  // Reinitialize for continuous listening
}

void i2c_init_slave(void) {
    I2C_Shutdown(MXC_I2C1);
    if (I2C_Init(MXC_I2C1, I2C_FAST_MODE, NULL) != E_NO_ERROR) {
        printf("Error initializing I2C\n");
        while (1);
    }
    I2C_SetSlaveAddr(MXC_I2C1, I2C_SLAVE_ADDR, 0);
    NVIC_EnableIRQ(I2C1_IRQn);
    i2c_slave_callback_init();
}

void i2c_slave_callback_init(void) {
    i2c_req_t i2c_slave_req;
    i2c_slave_req.addr = I2C_SLAVE_ADDR;
    i2c_slave_req.rx_data = NULL;
    i2c_slave_req.rx_len = 1;
    i2c_slave_req.callback = i2c_callback;
    I2C_SlaveAsync(MXC_I2C1, &i2c_slave_req);
}
