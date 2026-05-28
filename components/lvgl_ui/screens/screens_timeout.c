#include "screens_timeout.h"
#include "screens.h"
#include "bsp_display.h"
#include "lvgl.h"
#include "esp_log.h"

static const char *TAG = "screens_timeout";

static uint32_t s_timeout_to_main_ms = TIMEOUT_NEVER;
static uint32_t s_timeout_to_off_ms  = TIMEOUT_NEVER;
static bool     s_display_on         = true;

void disp_timeout_init(uint32_t timeout_to_main_ms, uint32_t timeout_to_off_ms)
{
    s_timeout_to_main_ms = timeout_to_main_ms;
    s_timeout_to_off_ms  = timeout_to_off_ms;
    s_display_on         = true;
}

void disp_timeout_set_off_ms(uint32_t timeout_ms)
{
    s_timeout_to_off_ms = timeout_ms;
}

bool disp_is_on(void)
{
    return s_display_on;
}

void disp_timeout_step(void)
{
    uint32_t inactive_ms = lv_disp_get_inactive_time(lv_disp_get_default());

    if (!s_display_on) {
        /* Any new activity wakes the display */
        if (inactive_ms < 100) {
            s_display_on = true;
            bsp_display_set_brightness(100);
            ESP_LOGD(TAG, "display on");
        }
        return;
    }

    /* Display-off timeout */
    if (s_timeout_to_off_ms != TIMEOUT_NEVER && inactive_ms >= s_timeout_to_off_ms) {
        s_display_on = false;
        bsp_display_set_brightness(0);
        ESP_LOGD(TAG, "display off");
        return;
    }

    /* Return-to-main timeout (ignored while already on the main screen) */
    if (s_timeout_to_main_ms != TIMEOUT_NEVER &&
        inactive_ms >= s_timeout_to_main_ms &&
        ui_screens_get_current() != SCREEN_MAIN) {
        ESP_LOGD(TAG, "timeout → main");
        ui_screens_navigate(SCREEN_MAIN);
    }
}
