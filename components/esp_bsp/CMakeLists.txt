file(GLOB_RECURSE SRC_FILES "*.c")

idf_component_register(SRCS ${SRC_FILES}
                    INCLUDE_DIRS "."
                    REQUIRES "esp_lcd" "driver" "esp_adc"
                             "fatfs" "nvs_flash" "lwip" "esp_wifi"
                             "lvgl__lvgl" "espressif__esp_lvgl_port"
                             "espressif__esp_lcd_touch_cst816s"
                             "espressif__button")
