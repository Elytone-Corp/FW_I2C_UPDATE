#include "i2c_master.h"
#include "i2c_definitions.h"
#include <stdio.h>
#include "mxc_config.h"
#include "i2c.h"
#include "mxc_delay.h"

int i2c_init_master(void) {
    // Configure I2C as master
    I2C_Shutdown(MXC_I2C0);
    if (I2C_Init(MXC_I2C0, I2C_FAST_MODE, NULL) != E_NO_ERROR) {
        printf("Error initializing I2C master\n");
        return -1;
    }
    return 0;
}

void i2c_send_data(void) {
    uint8_t tx_buf[1] = {0x55};  // Example data to send
    int error = I2C_MasterWrite(MXC_I2C0, I2C_SLAVE_ADDR, tx_buf, sizeof(tx_buf), 0);
    if (error != E_NO_ERROR) {
        printf("Error sending data to slave\n");
    } else {
        printf("Data sent to slave: 0x%02X\n", tx_buf[0]);
    }
}
