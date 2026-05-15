# Squareline Studio exports into the ui/ subdirectory.
# Re-export from Squareline by setting the export path to:
#   <project_root>/components/lvgl_ui/ui/
# This file is never overwritten by Squareline.

file(GLOB_RECURSE SRC_FILES "*.c")

idf_component_register(SRCS ${SRC_FILES}
                    INCLUDE_DIRS "."
                    PRIV_INCLUDE_DIRS "ui" "screens"
                    REQUIRES "lvgl__lvgl" "freertos" "esp_bsp" "espressif__esp_lvgl_port")
