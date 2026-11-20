#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H

#include "esp_err.h"
#include "iot_button.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_BUTTON_BOOT_GPIO    0

esp_err_t bsp_button_init(void);
esp_err_t bsp_button_register_cb(button_event_t event, button_cb_t cb, void *usr_data);

#ifdef __cplusplus
}
#endif

#endif
