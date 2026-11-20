#include "screens.h"
#include "scr_splash.h"
#include "scr_home.h"
#include "scr_sysinfo.h"
#include "scr_settings.h"
#include "nav_bar.h"
#include "ui.h"
#include "esp_log.h"

static const char *TAG = "screens";

static const screen_config_t screen_configs[SCREEN_MAX] = {
    [SCREEN_SPLASH]   = SCREEN_REGISTER("Splash",   ui_scrSplash,   splash),
    [SCREEN_HOME]     = SCREEN_REGISTER("Home",     ui_scrHome,     home),
    [SCREEN_SYSINFO]  = SCREEN_REGISTER("SysInfo",  ui_scrSysInfo,  sysinfo),
    [SCREEN_SETTINGS] = SCREEN_REGISTER("Settings", ui_scrSettings, settings),
};

static ui_screens_t s_active_screen = SCREEN_MAX;

void ui_screens_prepare(void)
{
    nav_bar_init();
    for (int i = 0; i < SCREEN_MAX; i++) {
        if (screen_configs[i].prepare) {
            screen_configs[i].prepare();
        }
    }
    if (ui_scrHome) {
        nav_bar_attach_swipe(ui_scrHome);
    }
}

ui_screens_t ui_screens_get_current(void)
{
    lv_obj_t *act = lv_scr_act();
    for (int i = 0; i < SCREEN_MAX; i++) {
        if (screen_configs[i].screen && *(screen_configs[i].screen) == act) {
            return (ui_screens_t)i;
        }
    }
    return SCREEN_MAX;
}

void ui_screens_navigate(ui_screens_t screen)
{
    if (screen >= SCREEN_MAX || !screen_configs[screen].screen) return;
    lv_obj_t *target = *(screen_configs[screen].screen);
    if (target && lv_scr_act() != target) {
        lv_scr_load_anim(target, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
    }
}

void ui_screens_step(void)
{
    ui_screens_t current = ui_screens_get_current();

    if (current != s_active_screen) {
        if (s_active_screen < SCREEN_MAX && screen_configs[s_active_screen].uninit) {
            screen_configs[s_active_screen].uninit();
        }
        if (current < SCREEN_MAX) {
            ESP_LOGI(TAG, "Screen -> %s", screen_configs[current].name);
            nav_bar_set_active(current);
            if (screen_configs[current].init) {
                screen_configs[current].init();
            }
        }
        s_active_screen = current;
    }

    if (current < SCREEN_MAX && screen_configs[current].step) {
        screen_configs[current].step();
    }
}

void ui_screens_on_activity(void)
{
    lv_disp_trig_activity(NULL);
}
