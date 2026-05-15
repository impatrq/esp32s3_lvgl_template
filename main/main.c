#include "esp_lvgl_port.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_touch.h"
#include "bsp_lvgl.h"

#include "ui.h"

/** @brief App entry point */
void app_main(void) {
    // Initialize necessary peripherals to handle display and touch
    i2c_master_bus_handle_t i2c_bus = bsp_i2c_init();
    bsp_display_init();
    bsp_touch_init(i2c_bus);
    ESP_ERROR_CHECK(bsp_lvgl_init());
    bsp_display_brightness_init();
    bsp_display_set_brightness(100);
    // Make sure to take LVGL mutex
    if (lvgl_port_lock(0)) {
        ui_init();
        lvgl_port_unlock();
    }
}