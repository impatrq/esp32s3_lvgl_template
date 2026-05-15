#include "screens.h"
#include "scr_main.h"
#include "ui.h"             /* Squareline export — screen object declarations */
#include "esp_log.h"

static const char *TAG = "screens";

/**
 * Screen registry.
 *
 * When you add a screen in Squareline Studio:
 *   1. Add its value to ui_screens_t in screens.h.
 *   2. Create scr_<name>.c/h in this directory.
 *   3. Add an entry below mapping the enum to the Squareline lv_obj_t* and
 *      your controller callbacks.
 *
 * The .screen_obj field must point to the extern lv_obj_t* variable that
 * Squareline declares in its generated header (e.g. &ui_Screen1).
 */
static const screen_config_t screen_configs[SCREEN_MAX] = {
    // [SCREEN_MAIN] = SCREEN_REGISTER("Main", ui_scrMain, main)
};

static ui_screens_t s_active_screen = SCREEN_MAX; /* SCREEN_MAX = "unknown" */

/* ------------------------------------------------------------------ */

void ui_screens_prepare(void)
{
    for (int i = 0; i < SCREEN_MAX; i++) {
        if (screen_configs[i].prepare) {
            screen_configs[i].prepare();
        }
    }
}

ui_screens_t ui_screens_get_current(void)
{
    lv_obj_t *active = lv_scr_act();
    for (int i = 0; i < SCREEN_MAX; i++) {
        if (screen_configs[i].screen_obj && *screen_configs[i].screen_obj == active) {
            return (ui_screens_t)i;
        }
    }
    return SCREEN_MAX;
}

void ui_screens_navigate(ui_screens_t screen)
{
    if (screen >= SCREEN_MAX) return;
    const screen_config_t *cfg = &screen_configs[screen];
    if (cfg->screen_obj && *cfg->screen_obj) {
        lv_disp_load_scr(*cfg->screen_obj);
    }
}

void ui_screens_on_activity(void)
{
    lv_disp_trig_activity(lv_disp_get_default());
}

void ui_screens_step(void)
{
    ui_screens_t current = ui_screens_get_current();

    /* Detect screen transition */
    if (current != s_active_screen) {
        if (s_active_screen < SCREEN_MAX && screen_configs[s_active_screen].uninit) {
            screen_configs[s_active_screen].uninit();
        }
        if (current < SCREEN_MAX) {
            ESP_LOGD(TAG, "→ %s", screen_configs[current].name);
            if (screen_configs[current].init) {
                screen_configs[current].init();
            }
        }
        s_active_screen = current;
    }

    /* Drive active screen */
    if (current < SCREEN_MAX && screen_configs[current].step) {
        screen_configs[current].step();
    }
}
