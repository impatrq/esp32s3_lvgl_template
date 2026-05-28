#include "lvgl_ui.h"
#include "ui.h"
#include "screens.h"
#include "screens_update.h"
#include "screens_timeout.h"

#define UI_STEP_PERIOD_MS 20

static void ui_step_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    ui_screens_step();
    ui_update_screens();
    disp_timeout_step();
}

void lvgl_ui_start(void)
{
    ui_update_init();
    disp_timeout_init(TIMEOUT_NEVER, TIMEOUT_NEVER);

    ui_init();
    ui_screens_prepare();

    ui_screens_navigate(SCREEN_SPLASH);

    lv_timer_create(ui_step_timer_cb, UI_STEP_PERIOD_MS, NULL);
}
