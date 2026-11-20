#include "esp_lvgl_port.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_button.h"
#include "screens/screens_update.h"

static void on_boot_btn_click(void *button_handle, void *usr_data)
{
    (void)button_handle;
    (void)usr_data;
    ui_update_cmd(UI_UPDATE_BTN_PRESS, NULL);
}
#include "bsp_touch.h"
#include "bsp_lvgl.h"

#include "lvgl_ui.h"

static void backlight_anim_cb(void *var, int32_t v)
{
    (void)var;
    bsp_display_set_brightness((uint8_t)v);
}

/** @brief App entry point */
void app_main(void) {
    // Initialize necessary peripherals to handle display and touch
    i2c_master_bus_handle_t i2c_bus = bsp_i2c_init();
    bsp_display_init();
    bsp_touch_init(i2c_bus);
    ESP_ERROR_CHECK(bsp_lvgl_init());
    bsp_display_brightness_init();
    ESP_ERROR_CHECK(bsp_button_init());
    ESP_ERROR_CHECK(bsp_button_register_cb(BUTTON_SINGLE_CLICK, on_boot_btn_click, NULL));
    bsp_display_set_brightness(0);

    // Always take LVGL mutex before doing anything with LVGL
    if (lvgl_port_lock(0)) {
        lvgl_ui_start();

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, backlight_anim_cb);
        lv_anim_set_values(&a, 0, 100);
        lv_anim_set_time(&a, 500);
        lv_anim_start(&a);

        lvgl_port_unlock();
    }
}