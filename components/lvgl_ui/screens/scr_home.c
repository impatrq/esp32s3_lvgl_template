#include <stdio.h>
#include "scr_home.h"
#include "screens.h"
#include "ui.h"
#include "bsp_battery.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"

static lv_timer_t *s_home_timer = NULL;
static temperature_sensor_handle_t s_temp_sensor = NULL;

static int voltage_to_percent(float voltage)
{
    if (voltage <= 3.30f) return 0;
    if (voltage >= 4.20f) return 100;
    return (int)(((voltage - 3.30f) / (4.20f - 3.30f)) * 100.0f);
}

static void update_home_widgets(void)
{
    char buf[32];

    float voltage = 0.0f;
    uint16_t adc_raw = 0;
    bsp_battery_get_voltage(&voltage, &adc_raw);
    int pct = voltage_to_percent(voltage);

    if (ui_arcBattery) {
        lv_arc_set_value(ui_arcBattery, pct);
        lv_color_t st_col = (pct > 50) ? lv_color_hex(0x3FB950) :
                            (pct > 20) ? lv_color_hex(0xD29922) :
                                         lv_color_hex(0xF85149);
        lv_obj_set_style_arc_color(ui_arcBattery, st_col, LV_PART_INDICATOR);
        if (ui_lblBattery) {
            lv_obj_set_style_text_color(ui_lblBattery, st_col, 0);
        }
    }
    if (ui_lblBattery) {
        snprintf(buf, sizeof(buf), "%d%%", pct);
        lv_label_set_text(ui_lblBattery, buf);
    }

    float temp_c = 0.0f;
    if (s_temp_sensor && temperature_sensor_get_celsius(s_temp_sensor, &temp_c) == ESP_OK) {
        if (ui_lblTemp) {
            snprintf(buf, sizeof(buf), "%.1f °C", temp_c);
            lv_label_set_text(ui_lblTemp, buf);
        }
    }

    int64_t total_sec = esp_timer_get_time() / 1000000LL;
    int hours   = (int)(total_sec / 3600);
    int minutes = (int)((total_sec % 3600) / 60);
    int seconds = (int)(total_sec % 60);

    if (ui_lblUptime) {
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);
        lv_label_set_text(ui_lblUptime, buf);
    }
}

static void home_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    update_home_widgets();
}

void scr_home_prepare(void)
{
    bsp_battery_init();

    if (s_temp_sensor == NULL) {
        temperature_sensor_config_t temp_cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
        if (temperature_sensor_install(&temp_cfg, &s_temp_sensor) == ESP_OK) {
            temperature_sensor_enable(s_temp_sensor);
        }
    }
}

void scr_home_init(void)
{
    update_home_widgets();

    if (s_home_timer == NULL) {
        s_home_timer = lv_timer_create(home_timer_cb, 1000, NULL);
    }
}

void scr_home_uninit(void)
{
    if (s_home_timer) {
        lv_timer_del(s_home_timer);
        s_home_timer = NULL;
    }

    if (s_temp_sensor) {
        temperature_sensor_disable(s_temp_sensor);
        temperature_sensor_uninstall(s_temp_sensor);
        s_temp_sensor = NULL;
    }
}

void scr_home_step(void)
{
}