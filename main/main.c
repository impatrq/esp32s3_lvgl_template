#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lvgl_port.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_touch.h"
#include "bsp_lvgl.h"

#include "lvgl_ui.h"

/** @brief App entry point */
void app_main(void) {
    // Initialize necessary peripherals to handle display and touch
    i2c_master_bus_handle_t i2c_bus = bsp_i2c_init();
    bsp_display_init();
    bsp_touch_init(i2c_bus);
    ESP_ERROR_CHECK(bsp_lvgl_init());
    bsp_display_brightness_init();
    bsp_display_set_brightness(0);
    // Always take LVGL mutex before doing anything with LVGL
    if (lvgl_port_lock(0)) {
        lvgl_ui_start();
        lvgl_port_unlock();
    }

    // Fade-in suave de retroiluminación al arrancar (0% -> 100%)
    for (uint8_t b = 0; b <= 100; b += 5) {
        bsp_display_set_brightness(b);
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}