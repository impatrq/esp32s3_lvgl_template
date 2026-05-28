#include "scr_home.h"
#include "screens.h"
#include "ui.h"
#include "lvgl.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"
#include "bsp_battery.h"

static const char *TAG = "scr_home";

static lv_obj_t *arc_battery;
static lv_obj_t *lbl_battery;
static lv_obj_t *lbl_temp;
static lv_obj_t *lbl_uptime;
static lv_timer_t *timer_update;
static temperature_sensor_handle_t temp_handle = NULL;

static void update_timer_cb(lv_timer_t *t)
{
    char buf[32];

    /* Batería */
    float voltage = 0;
    uint16_t raw = 0;
    bsp_battery_get_voltage(&voltage, &raw);
    int bat_pct = (int)((voltage - 3.0f) / 1.2f * 100.0f);
    if (bat_pct < 0)   bat_pct = 0;
    if (bat_pct > 100) bat_pct = 100;
    lv_arc_set_value(arc_battery, bat_pct);
    snprintf(buf, sizeof(buf), "%d%%", bat_pct);
    lv_label_set_text(lbl_battery, buf);
    lv_obj_align_to(lbl_battery, arc_battery, LV_ALIGN_CENTER, 0, 0);
    if (bat_pct > 50)
        lv_obj_set_style_arc_color(arc_battery, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    else if (bat_pct > 20)
        lv_obj_set_style_arc_color(arc_battery, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_INDICATOR);
    else
        lv_obj_set_style_arc_color(arc_battery, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

    /* Temperatura */
    float temp = 0;
    if (temp_handle) {
        temperature_sensor_get_celsius(temp_handle, &temp);
        snprintf(buf, sizeof(buf), "Temp: %.1f C", temp);
        lv_label_set_text(lbl_temp, buf);
    }

    /* Uptime */
    int64_t us = esp_timer_get_time();
    int total_s = (int)(us / 1000000);
    int hh = total_s / 3600;
    int mm = (total_s % 3600) / 60;
    int ss = total_s % 60;
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hh, mm, ss);
    lv_label_set_text(lbl_uptime, buf);
}

void scr_home_prepare(void) {}

void scr_home_init(void)
{
    ESP_LOGD(TAG, "init");

    bsp_battery_init();

    if (temp_handle == NULL) {
        temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 80);
        esp_err_t ret = temperature_sensor_install(&cfg, &temp_handle);
        if (ret == ESP_OK) {
            temperature_sensor_enable(temp_handle);
        } else {
            temp_handle = NULL;
        }
    }

    lv_obj_set_style_bg_color(ui_scrHome, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_scrHome, LV_OPA_COVER, LV_PART_MAIN);

    /* Arc batería */
    arc_battery = lv_arc_create(ui_scrHome);
    lv_arc_set_range(arc_battery, 0, 100);
    lv_arc_set_value(arc_battery, 0);
    lv_arc_set_bg_angles(arc_battery, 135, 45);
    lv_obj_set_size(arc_battery, 100, 100);
    lv_obj_align(arc_battery, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_remove_style(arc_battery, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_battery, LV_OBJ_FLAG_CLICKABLE);

    lbl_battery = lv_label_create(ui_scrHome);
    lv_label_set_text(lbl_battery, "0%");
    lv_obj_set_style_text_color(lbl_battery, lv_color_white(), 0);
    lv_obj_align_to(lbl_battery, arc_battery, LV_ALIGN_CENTER, 0, 0);

    /* Temperatura */
    lbl_temp = lv_label_create(ui_scrHome);
    lv_label_set_text(lbl_temp, "Temp: --");
    lv_obj_set_style_text_color(lbl_temp, lv_color_white(), 0);
    lv_obj_align(lbl_temp, LV_ALIGN_CENTER, 0, 20);

    /* Uptime */
    lbl_uptime = lv_label_create(ui_scrHome);
    lv_label_set_text(lbl_uptime, "00:00:00");
    lv_obj_set_style_text_color(lbl_uptime, lv_color_white(), 0);
    lv_obj_align(lbl_uptime, LV_ALIGN_CENTER, 0, 50);

    timer_update = lv_timer_create(update_timer_cb, 1000, NULL);
    lv_timer_ready(timer_update);
}

void scr_home_uninit(void)
{
    if (timer_update) {
        lv_timer_del(timer_update);
        timer_update = NULL;
    }
}

void scr_home_step(void) {}
