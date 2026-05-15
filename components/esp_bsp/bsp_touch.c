#include "bsp_touch.h"
#include "bsp_display.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch_axs5106.h"
#include "esp_log.h"

static const char *TAG = "bsp_touch";
static esp_lcd_touch_handle_t s_touch_handle = NULL;

void bsp_touch_init(i2c_master_bus_handle_t bus_handle)
{
    static i2c_master_dev_handle_t dev_handle;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ESP_LCD_TOUCH_IO_I2C_AXS5106_ADDRESS,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    esp_lcd_touch_config_t tp_cfg = {};
    tp_cfg.x_max = BSP_LCD_NATIVE_H_RES;
    tp_cfg.y_max = BSP_LCD_NATIVE_V_RES;
    tp_cfg.rst_gpio_num = BSP_PIN_TP_RST;
    tp_cfg.int_gpio_num = BSP_PIN_TP_INT;

#if BSP_DISPLAY_ROTATION == 90
    tp_cfg.flags.swap_xy = 1;
    tp_cfg.flags.mirror_x = 0;
    tp_cfg.flags.mirror_y = 0;
#elif BSP_DISPLAY_ROTATION == 180
    tp_cfg.flags.swap_xy = 0;
    tp_cfg.flags.mirror_x = 0;
    tp_cfg.flags.mirror_y = 1;
#elif BSP_DISPLAY_ROTATION == 270
    tp_cfg.flags.swap_xy = 1;
    tp_cfg.flags.mirror_x = 1;
    tp_cfg.flags.mirror_y = 1;
#else
    tp_cfg.flags.swap_xy = 0;
    tp_cfg.flags.mirror_x = 1;
    tp_cfg.flags.mirror_y = 0;
#endif

    ESP_LOGI(TAG, "Init touch controller");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_axs5106(dev_handle, &tp_cfg, &s_touch_handle));
}

esp_lcd_touch_handle_t bsp_touch_get_handle(void)
{
    return s_touch_handle;
}
