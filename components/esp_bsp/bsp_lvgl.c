#include "bsp_lvgl.h"
#include "bsp_display.h"
#include "bsp_touch.h"

#include "esp_lvgl_port.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "esp_check.h"
#include "ff.h"
#include "lvgl.h"

static const char *TAG = "bsp_lvgl";

static lv_disp_t *s_lvgl_disp = NULL;
static lv_indev_t *s_lvgl_touch_indev = NULL;

esp_err_t bsp_lvgl_init(void)
{
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 1024 * 10,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5,
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port init failed");

    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_display_get_io_handle(),
        .panel_handle = bsp_display_get_panel_handle(),
        .buffer_size = BSP_LCD_H_RES * BSP_LCD_DRAW_BUFF_HEIGHT,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_spiram = false,
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        },
    };

#if BSP_DISPLAY_ROTATION == 90
    disp_cfg.rotation.swap_xy = true;
    disp_cfg.rotation.mirror_x = true;
    disp_cfg.rotation.mirror_y = false;
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(bsp_display_get_panel_handle(), 0, 34));
#elif BSP_DISPLAY_ROTATION == 180
    disp_cfg.rotation.swap_xy = false;
    disp_cfg.rotation.mirror_x = true;
    disp_cfg.rotation.mirror_y = true;
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(bsp_display_get_panel_handle(), 34, 0));
#elif BSP_DISPLAY_ROTATION == 270
    disp_cfg.rotation.swap_xy = true;
    disp_cfg.rotation.mirror_x = false;
    disp_cfg.rotation.mirror_y = true;
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(bsp_display_get_panel_handle(), 0, 34));
#else
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(bsp_display_get_panel_handle(), 34, 0));
#endif

    s_lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = s_lvgl_disp,
        .handle = bsp_touch_get_handle(),
    };
    s_lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}

lv_disp_t *bsp_lvgl_get_display(void)
{
    return s_lvgl_disp;
}

lv_indev_t *bsp_lvgl_get_touch_indev(void)
{
    return s_lvgl_touch_indev;
}

/* ---- FatFS driver for LVGL ---- */

#define FS_FATFS_LETTER     'S'
#define FS_FATFS_CACHE_SIZE (20 * 1024)

static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    uint8_t flags = 0;
    if (mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
    else if (mode == LV_FS_MODE_RD) flags = FA_READ;
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

    FIL *f = (FIL *)lv_mem_alloc(sizeof(FIL));
    if (f == NULL) return NULL;

    FRESULT res = f_open(f, path, flags);
    if (res == FR_OK) return f;
    lv_mem_free(f);
    return NULL;
}

static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    LV_UNUSED(drv);
    f_close((FIL *)file_p);
    lv_mem_free(file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    LV_UNUSED(drv);
    FRESULT res = f_read((FIL *)file_p, buf, btr, (UINT *)br);
    return res == FR_OK ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    LV_UNUSED(drv);
    FRESULT res = f_write((FIL *)file_p, buf, btw, (UINT *)bw);
    return res == FR_OK ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    switch (whence) {
        case LV_FS_SEEK_SET: f_lseek((FIL *)file_p, pos); break;
        case LV_FS_SEEK_CUR: f_lseek((FIL *)file_p, f_tell((FIL *)file_p) + pos); break;
        case LV_FS_SEEK_END: f_lseek((FIL *)file_p, f_size((FIL *)file_p) + pos); break;
        default: break;
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    LV_UNUSED(drv);
    *pos_p = f_tell((FIL *)file_p);
    return LV_FS_RES_OK;
}

static void *fs_dir_open(lv_fs_drv_t *drv, const char *path)
{
    LV_UNUSED(drv);
    FF_DIR *d = (FF_DIR *)lv_mem_alloc(sizeof(FF_DIR));
    if (d == NULL) return NULL;
    FRESULT res = f_opendir(d, path);
    if (res != FR_OK) { lv_mem_free(d); return NULL; }
    return d;
}

static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *dir_p, char *fn)
{
    LV_UNUSED(drv);
    FRESULT res;
    FILINFO fno;
    fn[0] = '\0';
    do {
        res = f_readdir((FF_DIR *)dir_p, &fno);
        if (res != FR_OK) return LV_FS_RES_UNKNOWN;
        if (fno.fattrib & AM_DIR) { fn[0] = '/'; strcpy(&fn[1], fno.fname); }
        else strcpy(fn, fno.fname);
    } while (strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *dir_p)
{
    LV_UNUSED(drv);
    f_closedir((FF_DIR *)dir_p);
    lv_mem_free(dir_p);
    return LV_FS_RES_OK;
}

void bsp_lv_fs_fatfs_init(void)
{
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);
    fs_drv.letter = FS_FATFS_LETTER;
    fs_drv.cache_size = FS_FATFS_CACHE_SIZE;
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;
    fs_drv.dir_close_cb = fs_dir_close;
    lv_fs_drv_register(&fs_drv);
}
