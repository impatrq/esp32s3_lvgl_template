#include "scr_sysinfo.h"
#include "nav_bar.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include "esp_idf_version.h"
#include <stdio.h>

lv_obj_t *ui_scrSysInfo = NULL;
static lv_obj_t *s_lbl_heap = NULL;
static lv_obj_t *s_lbl_min_heap = NULL;
static uint32_t s_tracked_min_heap = UINT32_MAX;

void scr_sysinfo_reset_heap_tracker(void)
{
    s_tracked_min_heap = esp_get_free_heap_size();
    if (s_lbl_min_heap) {
        char buf[48];
        snprintf(buf, sizeof(buf), "Min Heap (session): %lu KB", (unsigned long)(s_tracked_min_heap / 1024));
        lv_label_set_text(s_lbl_min_heap, buf);
    }
}

void scr_sysinfo_prepare(void)
{
    ui_scrSysInfo = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_scrSysInfo, lv_color_hex(0x0D1117), 0);
    nav_bar_attach_swipe(ui_scrSysInfo);

    lv_obj_t *title = lv_label_create(ui_scrSysInfo);
    lv_label_set_text(title, "System Info");
    lv_obj_set_style_text_color(title, lv_color_hex(0x58A6FF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    esp_chip_info_t chip;
    esp_chip_info(&chip);

    char info_buf[128];
    snprintf(info_buf, sizeof(info_buf),
             "Chip: ESP32-S3 (%d cores, rev %d)\nIDF: %s",
             chip.cores, chip.revision, esp_get_idf_version());

    lv_obj_t *lbl_chip = lv_label_create(ui_scrSysInfo);
    lv_label_set_text(lbl_chip, info_buf);
    lv_obj_set_style_text_color(lbl_chip, lv_color_hex(0xC9D1D9), 0);
    lv_obj_align(lbl_chip, LV_ALIGN_TOP_LEFT, 16, 42);

    s_lbl_heap = lv_label_create(ui_scrSysInfo);
    lv_obj_set_style_text_color(s_lbl_heap, lv_color_hex(0x3FB950), 0);
    lv_obj_align(s_lbl_heap, LV_ALIGN_TOP_LEFT, 16, 95);

    s_lbl_min_heap = lv_label_create(ui_scrSysInfo);
    lv_obj_set_style_text_color(s_lbl_min_heap, lv_color_hex(0xD29922), 0);
    lv_obj_align(s_lbl_min_heap, LV_ALIGN_TOP_LEFT, 16, 122);

    lv_obj_t *hint = lv_label_create(ui_scrSysInfo);
    lv_label_set_text(hint, "[BOOT btn: reset min heap]");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x8B949E), 0);
    lv_obj_align(hint, LV_ALIGN_TOP_LEFT, 16, 152);
}

void scr_sysinfo_init(void)
{
    scr_sysinfo_step();
}

void scr_sysinfo_uninit(void) {}

void scr_sysinfo_step(void)
{
    uint32_t free_heap = esp_get_free_heap_size();
    if (free_heap < s_tracked_min_heap) {
        s_tracked_min_heap = free_heap;
    }

    char buf[48];
    if (s_lbl_heap) {
        snprintf(buf, sizeof(buf), "Free Heap: %lu KB", (unsigned long)(free_heap / 1024));
        lv_label_set_text(s_lbl_heap, buf);
    }
    if (s_lbl_min_heap) {
        snprintf(buf, sizeof(buf), "Min Heap (session): %lu KB", (unsigned long)(s_tracked_min_heap / 1024));
        lv_label_set_text(s_lbl_min_heap, buf);
    }
}
