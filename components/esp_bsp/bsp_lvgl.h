#ifndef __BSP_LVGL_H__
#define __BSP_LVGL_H__

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_lvgl_init(void);
lv_disp_t *bsp_lvgl_get_display(void);
lv_indev_t *bsp_lvgl_get_touch_indev(void);

/* Optional: register LVGL FatFS driver (call after bsp_sdcard_init) */
void bsp_lv_fs_fatfs_init(void);

#ifdef __cplusplus
}
#endif

#endif
