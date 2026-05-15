#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "driver/i2c_master.h"
#include "driver/gpio.h"

#define BSP_PIN_I2C_SDA   GPIO_NUM_42
#define BSP_PIN_I2C_SCL   GPIO_NUM_41
#define BSP_I2C_PORT_NUM  0

#ifdef __cplusplus
extern "C" {
#endif

i2c_master_bus_handle_t bsp_i2c_init(void);


#ifdef __cplusplus
}
#endif



#endif //__BSP_I2C_H__