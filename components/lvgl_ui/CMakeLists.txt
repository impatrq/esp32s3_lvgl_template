file(GLOB_RECURSE SRC_FILES "*.c" "ui/*.c" "screens/*.c")
idf_component_register(SRCS ${SRC_FILES}
                    INCLUDE_DIRS "." "ui" "screens"
                    REQUIRES "lvgl__lvgl" "freertos" "esp_bsp" "espressif__esp_lvgl_port")
